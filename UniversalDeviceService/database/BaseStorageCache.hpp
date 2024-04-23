#pragma once

#include <mutex>

#include "IQueryExecutor.hpp"
#include "IStorageCache.hpp"

class BaseStorageCache : public IStorageCache {
public:
    BaseStorageCache(IQueryExecutor* queryExecutor);

    virtual ~BaseStorageCache() = default;

protected:
    IQueryExecutor* _queryExecutor;
    std::mutex _mutex;
};
