#include "Storage.h"

#include <iostream>
#include <vector>

#include "PathHelper.h"

int NoActionCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
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

void Storage::Execute(const std::string& query)
{
    Execute(query, NoActionCallback);
}

void Storage::Execute(const std::string& query, int(*callback)(void*, int, char**, char**))
{
    char* error = nullptr;
    int result = sqlite3_exec(_connection, query.c_str(), callback, this, &error);
    if (result != SQLITE_OK)
    {   
        std::cout << "SQL error: " << error << std::endl;
        sqlite3_free(error);
    }
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
