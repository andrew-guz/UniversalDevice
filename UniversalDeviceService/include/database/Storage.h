#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <mutex>
#include <sqlite3.h>

#include "IQueryExecutor.h"

class Storage final : public IQueryExecutor {
public:
    Storage();

    virtual ~Storage();

    virtual bool Execute(std::string_view query) override;

    virtual bool Execute(std::string_view query, int (*callback)(void*, int, char**, char**)) override;

    virtual bool Select(std::string_view query, std::vector<std::vector<std::string>>& data) override;

    virtual bool Delete(std::string query) override;

    virtual std::vector<std::string> GetAllTables() const override;

    virtual std::vector<std::string> GetDeviceRelatedTables() const override;

private:
    bool InternalExecute(std::string_view query, int (*callback)(void*, int, char**, char**), void* data);

    void InitializeDb();

private:
    sqlite3* _connection = nullptr;
    std::mutex _mutex;
};

#endif //_STORAGE_H_
