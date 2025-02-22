#include "Storage.hpp"

#include <chrono>

#include <fmt/format.h>

#include "Logger.hpp"

namespace {
    constexpr std::chrono::weeks oldDataDelta = std::chrono::weeks{ 2 };
}

static std::map<std::string, std::string> Tables = {
    { "Devices", "CREATE TABLE IF NOT EXISTS Devices (id TEXT UNIQUE, type TEXT, name TEXT, grp TEXT, timestamp INTEGER, PRIMARY KEY(id, type))" },
    { "Settings", "CREATE TABLE IF NOT EXISTS Settings (id TEXT, settings TEXT, PRIMARY KEY(id))" },
    { "Commands", "CREATE TABLE IF NOT EXISTS Commands (id TEXT, commands TEXT, PRIMARY KEY(id))" },
    { "Events",
      "CREATE TABLE IF NOT EXISTS Events (id TEXT UNIQUE, active INTEGER, providerId TEXT, providerType TEXT, event TEXT, PRIMARY KEY(id))" },
    { "Thermometers",
      "CREATE TABLE IF NOT EXISTS Thermometers (idx INTEGER, id TEXT, timestamp INTEGER, value REAL, PRIMARY KEY(idx AUTOINCREMENT))" },
    { "Relays", "CREATE TABLE IF NOT EXISTS Relays (idx INTEGER, id TEXT, timestamp INTEGER, state INTEGER, PRIMARY KEY(idx AUTOINCREMENT))" },
    { "MotionRelays",
      "CREATE TABLE IF NOT EXISTS MotionRelays (idx INTEGER, id TEXT, timestamp INTEGER, motion INTEGER, state INTEGER, PRIMARY "
      "KEY(idx AUTOINCREMENT))" },
    { "Scenarios", "CREATE TABLE IF NOT EXISTS Scenarios (id TEXT, scenarios TEXT, PRIMARY KEY(id))" },
};

int NoActionCallback(void* data, int columnsInRow, char** rowData, char** columnNames) { return 0; }

int SelectCallback(void* data, int columnsInRow, char** rowData, char** columnNames) {
    auto result = (std::vector<std::vector<std::string>>*)data;
    std::vector<std::string> row;
    for (int column = 0; column < columnsInRow; ++column) {
        auto columnName = columnNames[column];
        auto rawCellData = rowData[column];
        auto cellData = rawCellData ? std::string(rawCellData) : std::string();
        row.push_back(columnName);
        row.push_back(cellData);
    }
    result->push_back(row);
    return 0;
}

Storage::Storage(const std::filesystem::path& dbPath) {
    sqlite3_open(PathHelper::FullFilePath(dbPath).c_str(), &_connection);
    InitializeDb();
}

Storage::~Storage() { sqlite3_close(_connection); }

bool Storage::Begin() { return Execute("BEGIN TRANSACTION;"); }

bool Storage::Execute(const std::string_view query) { return Execute(query, NoActionCallback); }

bool Storage::Execute(const std::string_view query, int (*callback)(void*, int, char**, char**)) { return InternalExecute(query, callback, this); }

bool Storage::Select(const std::string_view query, std::vector<std::vector<std::string>>& data) {
    return InternalExecute(query, SelectCallback, &data);
}

bool Storage::Delete(std::string query) { return Execute(query, NoActionCallback); }

bool Storage::Commit() { return Execute("COMMIT;"); }

std::vector<std::string> Storage::GetAllTables() const {
    return {
        "Devices", "Settings", "Commands", "Events", "Thermometers", "Relays", "MotionRelays", "Scenarios",
    };
}

std::vector<std::string> Storage::GetDeviceRelatedTables() const {
    return {
        "Devices",
        "Thermometers",
        "Relays",
        "MotionRelays",
    };
}

std::vector<std::string> Storage::GetDataTables() const {
    return {
        "Thermometers",
        "Relays",
        "MotionRelays",
    };
}

void Storage::CleanupOldData(const std::chrono::system_clock::time_point& timestamp) {
    const std::chrono::system_clock::time_point oldTimestamp = timestamp - oldDataDelta;
    for (const std::string& table : GetDataTables()) {
        const std::string query = fmt::format("DELETE FROM {} WHERE timestamp < {}", table, TimeHelper::TimeToInt(oldTimestamp));
        Delete(query);
    }
}

bool Storage::InternalExecute(const std::string_view query, int (*callback)(void*, int, char**, char**), void* data) {
    std::lock_guard<std::mutex> lockGuard(_mutex);
    char* error = nullptr;
    int result = sqlite3_exec(_connection, query.data(), callback, data, &error);
    if (result != SQLITE_OK) {
        LOG_ERROR_MSG(fmt::format("SQL error: {} for query {}", error, query));
        sqlite3_free(error);
        return false;
    }
    return true;
}

void Storage::InitializeDb() {
    for (const auto& pair : Tables)
        Execute(pair.second);
}
