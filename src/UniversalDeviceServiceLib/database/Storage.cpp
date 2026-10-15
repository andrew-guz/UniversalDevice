#include "Storage.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <boost/hof/match.hpp>
#include <fmt/format.h>

#include "Constants.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "PathHelper.hpp"
#include "TimeHelper.hpp"

static std::map<std::string, std::string> Tables = {
    { "Settings", "CREATE TABLE IF NOT EXISTS Settings (id TEXT, settings TEXT, PRIMARY KEY(id))" },
    { "Commands", "CREATE TABLE IF NOT EXISTS Commands (id TEXT, command TEXT, PRIMARY KEY(id))" },
    { "Devices", "CREATE TABLE IF NOT EXISTS Devices (id TEXT UNIQUE, type TEXT, name TEXT, grp TEXT, timestamp INTEGER, PRIMARY KEY(id, type))" },
    { "Events", "CREATE TABLE IF NOT EXISTS Events (id TEXT UNIQUE, event TEXT, PRIMARY KEY(id))" },
    { "Scenarios", "CREATE TABLE IF NOT EXISTS Scenarios (id TEXT, scenario TEXT, PRIMARY KEY(id))" },
    { "Thermometers",
      "CREATE TABLE IF NOT EXISTS Thermometers (idx INTEGER, id TEXT, timestamp INTEGER, value REAL, PRIMARY KEY(idx AUTOINCREMENT))" },
    { "Relays", "CREATE TABLE IF NOT EXISTS Relays (idx INTEGER, id TEXT, timestamp INTEGER, state INTEGER, PRIMARY KEY(idx AUTOINCREMENT))" },
    { "MotionRelays",
      "CREATE TABLE IF NOT EXISTS MotionRelays (idx INTEGER, id TEXT, timestamp INTEGER, motion INTEGER, state INTEGER, PRIMARY "
      "KEY(idx AUTOINCREMENT))" },
    { "UniversalDevices",
      "CREATE TABLE IF NOT EXISTS UniversalDevices (idx INTEGER, id TEXT, timestamp INTEGER, 'values' TEXT, PRIMARY KEY(idx AUTOINCREMENT))" },
};

Storage::Storage(const std::filesystem::path& dbPath) {
    _dbPath = dbPath;
    sqlite3_open(PathHelper::FullFilePath(dbPath).c_str(), &_connection);
    sqlite3_busy_timeout(_connection, 5000);
    InitializeDb();
}

Storage::~Storage() { sqlite3_close(_connection); }

std::vector<std::string> Storage::GetAllTables() const {
    return {
        "Devices", "Settings", "Commands", "Events", "Thermometers", "Relays", "MotionRelays", "Scenarios", "UniversalDevices",
    };
}

std::vector<std::string> Storage::GetDeviceRelatedTables() const {
    return {
        "Devices", "Thermometers", "Relays", "MotionRelays", "UniversalDevices",
    };
}

std::vector<std::string> Storage::GetDataTables() const {
    return {
        "Thermometers",
        "Relays",
        "MotionRelays",
        "UniversalDevices",
    };
}

void Storage::CleanupOldData(const std::chrono::system_clock::time_point& timestamp) {
    for (const std::string& table : GetDataTables()) {
        const std::string query = fmt::format("DELETE FROM {} WHERE timestamp < ?", table);
        if (!NewDelete(query, TimeHelper::TimeToInt(timestamp))) {
            LOG_SQL_ERROR(query);
        }
    }
}

std::int64_t Storage::NewInsertImpl(std::string_view query, const DatabaseValues& databaseValues) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::unique_ptr<Storage::Query> queryPtr = CreateQuery(query);
    if (queryPtr == nullptr)
        return Constants::InvalidId;

    const int bindResult = BindQuery(queryPtr->_statement, databaseValues);
    if (bindResult != SQLITE_OK) {
        LOG_ERROR_MSG(fmt::format("SQL bind error: {} for query {}", sqlite3_errmsg(_connection), query));
        return Constants::InvalidId;
    }

    if (sqlite3_step(queryPtr->_statement) == SQLITE_DONE) {
        return sqlite3_last_insert_rowid(_connection);
    }

    LOG_ERROR_MSG(fmt::format("SQL INSERT error: {} for query {}", sqlite3_errmsg(_connection), query));

    return Constants::InvalidId;
}

bool Storage::NewSelectImpl(std::vector<std::vector<std::string>>& data, std::string_view query, const DatabaseValues& databaseValues) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    data.clear();

    std::unique_ptr<Storage::Query> queryPtr = CreateQuery(query);
    if (queryPtr == nullptr)
        return false;

    const int bindResult = BindQuery(queryPtr->_statement, databaseValues);
    if (bindResult != SQLITE_OK) {
        LOG_ERROR_MSG(fmt::format("SQL bind error: {} for query {}", sqlite3_errmsg(_connection), query));
        return false;
    }

    const int columnCount = sqlite3_column_count(queryPtr->_statement);
    std::vector<std::string> columnNames;
    columnNames.reserve(columnCount);
    for (int column = 0; column < columnCount; ++column)
        columnNames.emplace_back(sqlite3_column_name(queryPtr->_statement, column));

    int result = SQLITE_ERROR;
    do {
        result = sqlite3_step(queryPtr->_statement);
        if (result == SQLITE_DONE) {
            return true;
        } else if (result == SQLITE_ROW) {
            std::vector<std::string> row;
            row.reserve(columnCount * 2);
            for (int column = 0; column < columnCount; ++column) {
                row.push_back(columnNames[column]);

                const auto* text = reinterpret_cast<const char*>(sqlite3_column_text(queryPtr->_statement, column));
                if (text != nullptr)
                    row.emplace_back(text, sqlite3_column_bytes(queryPtr->_statement, column));
                else
                    row.emplace_back();
            }

            data.push_back(std::move(row));
        } else {
            LOG_ERROR_MSG(fmt::format("SQL SELECT error: {} for query {}", sqlite3_errmsg(_connection), query));
            return false;
        }
    } while (result == SQLITE_ROW);

    return true;
}

bool Storage::NewUpdateImpl(const std::string_view query, const DatabaseValues& databaseValues) {
    return ExecuteRaw(query, databaseValues, "UPDATE");
}

bool Storage::NewDeleteImpl(const std::string_view query, const DatabaseValues& databaseValues) {
    return ExecuteRaw(query, databaseValues, "DELETE");
}

bool Storage::Begin() { return ExecuteRaw("BEGIN TRANSACTION;", {}, "BEGIN TRANSACTION"); }

bool Storage::Rollback() { return ExecuteRaw("ROLLBACK;", {}, "ROLLBACK"); }

bool Storage::Commit() { return ExecuteRaw("COMMIT;", {}, "COMMIT"); }

bool Storage::ExecuteRaw(const std::string_view query, const DatabaseValues& databaseValues, const std::string_view errorMsg) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::unique_ptr<Storage::Query> queryPtr = CreateQuery(query);
    if (queryPtr == nullptr)
        return false;

    const int bindResult = BindQuery(queryPtr->_statement, databaseValues);
    if (bindResult != SQLITE_OK) {
        LOG_ERROR_MSG(fmt::format("SQL bind error: {} for query {}", sqlite3_errmsg(_connection), query));
        return false;
    }

    if (sqlite3_step(queryPtr->_statement) == SQLITE_DONE) {
        return true;
    }

    LOG_ERROR_MSG(fmt::format("SQL {} error: {} for query {}", errorMsg, sqlite3_errmsg(_connection), query));

    return false;
}

std::unique_ptr<Storage::Query> Storage::CreateQuery(const std::string_view query) {
    sqlite3_stmt* statement = nullptr;

    const int prepareResult = sqlite3_prepare_v2(_connection, query.data(), static_cast<int>(query.size()), &statement, nullptr);
    if (prepareResult != SQLITE_OK) {
        LOG_ERROR_MSG(fmt::format("SQL prepare error: {} for query {}", sqlite3_errmsg(_connection), query));
        return nullptr;
    }

    return std::make_unique<Storage::Query>(statement);
}

int Storage::BindQuery(sqlite3_stmt* statement, const DatabaseValues& databaseValues) {
    int result = SQLITE_OK;
    for (std::size_t index = 0; index < databaseValues.size() && result == SQLITE_OK; ++index) {
        // IMPORTANT: bind index starts with 1
        result = std::visit(boost::hof::match(
                                [&](const std::string& value) -> int {
                                    return sqlite3_bind_text(statement, index + 1, value.c_str(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
                                },
                                [&](const std::int64_t& value) -> int { return sqlite3_bind_int64(statement, index + 1, value); },
                                [&](const float& value) -> int { return sqlite3_bind_double(statement, index + 1, value); }),
                            databaseValues[index]);
    }

    return result;
}

void Storage::InitializeDb() {
    for (const auto& [_, query] : Tables)
        ExecuteRaw(query, {}, "CREATE TABLE");
}
