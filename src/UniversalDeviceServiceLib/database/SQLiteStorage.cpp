#include "SQLiteStorage.hpp"

#include <sqlite3.h>

#include <fmt/format.h>

#include "Logger.hpp"

namespace {
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

} // namespace

SQLiteStorage::SQLiteStorage(const std::filesystem::path& dbPath) :
    Storage() {
    _dbPath = dbPath;
    sqlite3_open(PathHelper::FullFilePath(dbPath).c_str(), &_connection);
    InitializeDb();
}

SQLiteStorage::~SQLiteStorage() { sqlite3_close(_connection); }

bool SQLiteStorage::Begin() { return Execute("BEGIN TRANSACTION;"); }

bool SQLiteStorage::Execute(const std::string_view query) { return Execute(query, NoActionCallback); }

bool SQLiteStorage::Execute(const std::string_view query, int (*callback)(void*, int, char**, char**)) {
    return InternalExecute(query, callback, this);
}

bool SQLiteStorage::Select(const std::string_view query, std::vector<std::vector<std::string>>& data) {
    return InternalExecute(query, SelectCallback, &data);
}

bool SQLiteStorage::Delete(const std::string_view query) { return Execute(query, NoActionCallback); }

bool SQLiteStorage::Commit() { return Execute("COMMIT;"); }

bool SQLiteStorage::InternalExecute(const std::string_view query, int (*callback)(void*, int, char**, char**), void* data, const int repeatCount) {
    _mutex.lock();
    char* error = nullptr;
    int result = sqlite3_exec(_connection, query.data(), callback, data, &error);
    if (result != SQLITE_OK) {
        LOG_ERROR_MSG(fmt::format("SQL error: {} for query {}", error, query));
        sqlite3_free(error);

        if (result == SQLITE_CANTOPEN && repeatCount == 0) {
            // let's close and reopen connection and try again
            LOG_INFO_MSG("Reconnecting to database...");
            int result = sqlite3_close(_connection);
            LOG_INFO_MSG(fmt::format("Close result: {}", result));
            _connection = nullptr;
            result = sqlite3_open(PathHelper::FullFilePath(_dbPath).c_str(), &_connection);
            LOG_INFO_MSG(fmt::format("Open result: {}", result));
            _mutex.unlock();
            return InternalExecute(query, callback, data, 1);
        }

        _mutex.unlock();
        return false;
    }

    _mutex.unlock();
    return true;
}
