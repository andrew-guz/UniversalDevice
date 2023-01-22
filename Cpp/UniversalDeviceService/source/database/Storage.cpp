#include "Storage.h"

#include <iostream>

#include "PathHelper.h"

int NoActionCallback(void* data, int columnsInRow, char** rowData, char** columnName)
{
    return 0;
}

int SelectCallback(void* data, int columnsInRow, char** rowData, char** columnName)
{
    auto result = (std::vector<std::vector<std::string>>*)data;
    std::vector<std::string> row;
    for (int column = 0; column < columnsInRow; ++column)
        row.push_back(rowData[column]);
    result->push_back(row);
    return 0;
}

std::string DbPath()
{
    return PathHelper::AppPath() + std::string(".db");
}

Storage::Storage()
{
    sqlite3_open(DbPath().c_str(), &_connection);
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
    char* error = nullptr;
    int result = sqlite3_exec(_connection, query.c_str(), callback, data, &error);
    if (result != SQLITE_OK)
    {   
        std::cout << "SQL error: " << error << std::endl;
        sqlite3_free(error);
        return false;
    }
    return true;
}

void Storage::InitializeDb()
{
    std::vector<std::string> queries
    {
        "CREATE TABLE IF NOT EXISTS 'Devices' ('id' TEXT UNIQUE, 'type' TEXT, 'name' TEXT, 'timestamp' TEXT, PRIMARY KEY('id', 'type'))",
        "CREATE TABLE IF NOT EXISTS 'Thermometers' ('id' TEXT, 'timestamp' TEXT, 'value' REAL)"
    };
    for(auto& query : queries)
        Execute(query);
}
