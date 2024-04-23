#include "BaseStorageCache.hpp"

BaseStorageCache::BaseStorageCache(IQueryExecutor* queryExecutor) : IStorageCache(), _queryExecutor(queryExecutor) {}
