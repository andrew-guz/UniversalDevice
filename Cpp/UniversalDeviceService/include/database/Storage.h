#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <string>
#include <sqlite3.h>

class Storage final
{
public:
    Storage();

    ~Storage();

    void Execute(const std::string& query);

    void Execute(const std::string& query, int(*callback)(void*, int, char**, char**));

private:
    void InitializeDb();

private:
    sqlite3* _connection = nullptr;
};

#endif //_STORAGE_H_
