#include "Storage.h"

#include "Logger.h"

int NoActionCallback(void* data, int columnsInRow, char** rowData, char** columnNames)
{
    return 0;
}

int SelectCallback(void* data, int columnsInRow, char** rowData, char** columnNames)
{
    auto result = (std::vector<std::vector<std::string>>*)data;
    std::vector<std::string> row;
    for (int column = 0; column < columnsInRow; ++column)
    {
        auto columnName = columnNames[column];
        auto rawCellData = rowData[column];       
        auto cellData = rawCellData ? std::string(rawCellData) : std::string();
        row.push_back(columnName);
        row.push_back(cellData);
    }
    result->push_back(row);
    return 0;
}

Storage::Storage()
{
    sqlite3_open(PathHelper::AppDbPath().c_str(), &_connection);
    InitializeDb();
}

Storage::~Storage()
{
    sqlite3_close(_connection);
}

bool Storage::Execute(const std::string& query)
{
    return Execute(query, NoActionCallback);
}

bool Storage::Execute(const std::string& query, int(*callback)(void*, int, char**, char**))
{
    return InternalExecute(query, callback, this);
}

bool Storage::Select(const std::string& query, std::vector<std::vector<std::string>>& data)
{
    return InternalExecute(query, SelectCallback, &data);
}

bool Storage::InternalExecute(const std::string& query, int(*callback)(void*, int, char**, char**), void* data)
{
    std::lock_guard<std::mutex> lock(_mutex);
    char* error = nullptr;
    int result = sqlite3_exec(_connection, query.c_str(), callback, data, &error);
    if (result != SQLITE_OK)
    {   
        LOG_ERROR << "SQL error: " << error << " for query " << query << "." << std::endl;
        sqlite3_free(error);
        return false;
    }
    return true;
}

void Storage::InitializeDb()
{
    std::vector<std::string> queries
    {
        "CREATE TABLE IF NOT EXISTS Devices (id TEXT UNIQUE, type TEXT, name TEXT, timestamp TEXT, PRIMARY KEY(id, type))",
        "CREATE TABLE IF NOT EXISTS Settings (id TEXT, settings TEXT, PRIMARY KEY(id))",
        "CREATE TABLE IF NOT EXISTS Commands (id TEXT, commands TEXT, PRIMARY KEY(id))",
        "CREATE TABLE IF NOT EXISTS Events (id TEXT, type TEXT, active INTEGER, event TEXT)",
        "CREATE TABLE IF NOT EXISTS Thermometers (idx INTEGER, id TEXT, timestamp TEXT, value REAL, PRIMARY KEY(idx AUTOINCREMENT))",
        "CREATE TABLE IF NOT EXISTS Relays (idx INTEGER, id TEXT, timestamp TEXT, state INTEGER, PRIMARY KEY(idx AUTOINCREMENT))"
    };
    for(auto& query : queries)
        Execute(query);
}
