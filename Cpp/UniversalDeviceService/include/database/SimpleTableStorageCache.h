#ifndef _SIMPLE_TABLE_STORAGE_CACHE_H_
#define _SIMPLE_TABLE_STORAGE_CACHE_H_

#include <string>
#include <mutex>
#include <map>

#include "IStorageCache.h"
#include "IQueryExecutor.h"

class SimpleTableStorageCache final : public IStorageCache
{
public:
    SimpleTableStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName);

    virtual ~SimpleTableStorageCache() = default;    

    virtual std::tuple<std::string, StorageCacheSharedData::Problem> Select(const std::string& id) override;

    virtual StorageCacheSharedData::Problem InsertOrReplace(const std::string& id, const std::string& data) override;

private:
    IQueryExecutor*                     _queryExecutor;
    std::string                         _tableName;
    std::string                         _fieldName;
    std::mutex                          _mutex;
    std::map<std::string, std::string>  _dataCache; 
};

#endif //_SIMPLE_TABLE_STORAGE_CACHE_H_
