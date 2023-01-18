#include "Storage.h"

#include "PathHelper.h"

std::string DbPath()
{
    return PathHelper::AppPath() + std::string(".db");
}

Storage::Storage()
{
    sqlite3_open(DbPath().c_str(), &_connection);
}

Storage::~Storage()
{
    sqlite3_close(_connection);
}
