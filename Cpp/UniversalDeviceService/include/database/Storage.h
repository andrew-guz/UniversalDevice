#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <sqlite3.h>
#include <mutex>

#include "IQueryExecutor.h"

class Storage final : public IQueryExecutor
{
public:
    Storage();

    virtual ~Storage();

    virtual bool Execute(const std::string& query) override;

    virtual bool Execute(const std::string& query, int(*callback)(void*, int, char**, char**)) override;

    virtual bool Select(const std::string& query, std::vector<std::vector<std::string>>& data) override;    

private:
    bool InternalExecute(const std::string& query, int(*callback)(void*, int, char**, char**), void* data);

    void InitializeDb();

private:
    sqlite3*    _connection = nullptr;
    std::mutex  _mutex;
};

#endif //_STORAGE_H_
