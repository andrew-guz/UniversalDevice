#ifndef _BASE_STORAGE_CACHE_H_
#define _BASE_STORAGE_CACHE_H_

#include <mutex>

#include "IStorageCache.h"
#include "IQueryExecutor.h"

class BaseStorageCache : public IStorageCache
{
public:
    BaseStorageCache(IQueryExecutor* queryExecutor);

    virtual ~BaseStorageCache() = default;

protected:
    IQueryExecutor* _queryExecutor;
    std::mutex      _mutex;
};

#endif // _BASE_STORAGE_CACHE_H_
