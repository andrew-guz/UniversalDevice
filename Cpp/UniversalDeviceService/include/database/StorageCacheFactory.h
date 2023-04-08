#ifndef _STORAGE_CACHE_FACTORY_H_
#define _STORAGE_CACHE_FACTORY_H_

#include <mutex>
#include <string>
#include <map>
#include <tuple>

#include "Singleton.h"
#include "IStorageCache.h"
#include "IQueryExecutor.h"

class StorageCacheFactory final : public Singleton<StorageCacheFactory>
{
public:
    IStorageCache* GetStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName);

private:
    std::mutex                                                      _mutex;
    std::map<std::tuple<std::string, std::string>, IStorageCache*>  _cache;
};

#endif //_STORAGE_CACHE_FACTORY_H_
