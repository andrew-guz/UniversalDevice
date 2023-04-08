#include "BaseStorageCache.h"

BaseStorageCache::BaseStorageCache(IQueryExecutor* queryExecutor) :
    IStorageCache(),
    _queryExecutor(queryExecutor)
{
}
