#pragma once

#include <map>
#include <mutex>
#include <string>

#include "IQueryExecutor.hpp"
#include "IStorageCache.hpp"
#include "Singleton.hpp"

class StorageCacheFactory final : public Singleton<StorageCacheFactory> {
public:
    template<typename T>
    IStorageCache* GetStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName) {
        const std::lock_guard<std::mutex> lock(_mutex);

        auto key = CreateKey(tableName, fieldName);
        auto iter = _cache.find(key);
        if (iter != _cache.end())
            return iter->second;

        auto storageCache = new T(queryExecutor, tableName, fieldName);
        _cache.insert(std::pair<std::string, IStorageCache*>(key, storageCache));

        return storageCache;
    }

    template<typename T, bool useTableName>
    IStorageCache* GetStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName) {
        const std::lock_guard<std::mutex> lock(_mutex);

        auto key = CreateKey(tableName, {});
        auto iter = _cache.find(key);
        if (iter != _cache.end())
            return iter->second;

        T* storageCache = nullptr;
        if constexpr (useTableName) {
            storageCache = new T(queryExecutor, tableName);
        } else {
            storageCache = new T(queryExecutor);
        }
        _cache.insert(std::pair<std::string, IStorageCache*>(key, storageCache));

        return storageCache;
    }

private:
    std::string CreateKey(const std::string& tableName, const std::string& fieldName);

private:
    std::mutex _mutex;
    std::map<std::string, IStorageCache*> _cache;
};
