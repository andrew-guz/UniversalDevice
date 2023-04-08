#include "StorageCacheFactory.h"

#include "SimpleTableStorageCache.h"

IStorageCache* StorageCacheFactory::GetStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto key = std::make_tuple(tableName, fieldName);
    auto iter = _cache.find(key);
    if (iter != _cache.end())
        return iter->second;
    auto storageCache = new SimpleTableStorageCache(queryExecutor, tableName, fieldName);
    _cache.insert(std::pair<std::tuple<std::string, std::string>, IStorageCache*>(key, storageCache));
    return storageCache;
}
