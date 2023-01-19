#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <sqlite3.h>

#include "IQueryExecutor.h"

class Storage final : public IQueryExecutor
{
public:
    Storage();

    virtual ~Storage();

    virtual void Execute(const std::string& query) override;

    virtual void Execute(const std::string& query, int(*callback)(void*, int, char**, char**)) override;

private:
    void InitializeDb();

private:
    sqlite3* _connection = nullptr;
};

#endif //_STORAGE_H_
