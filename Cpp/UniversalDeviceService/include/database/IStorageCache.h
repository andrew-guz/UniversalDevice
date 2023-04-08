#ifndef _I_STORAGE_CACHE_H_
#define _I_STORAGE_CACHE_H_

#include "StorageCacheSharedData.h"

class IStorageCache
{
public:
    virtual ~IStorageCache() = default;

    virtual std::tuple<std::string, StorageCacheSharedData::Problem> Select(const std::string& id) = 0;

    virtual StorageCacheSharedData::Problem InsertOrReplace(const std::string& id, const std::string& data) = 0;
};

#endif //_I_STORAGE_CACHE_H_
