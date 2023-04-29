#ifndef _NIGHT_MODE_TABLE_STORAGE_CACHE_H_
#define _NIGHT_MODE_TABLE_STORAGE_CACHE_H_

#include <map>

#include "BaseStorageCache.h"
#include "NightModeDevice.h"

class NightModeTableStorageCache final : public BaseStorageCache
{
public:
    NightModeTableStorageCache(IQueryExecutor* queryExecutor);

    virtual ~NightModeTableStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) override;

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override;

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override;

    virtual StorageCacheProblem Update(const UpdateInput& what) override;

    virtual StorageCacheProblem Delete(const DeleteInput& what) override;

private:
    std::map<ComponentDescription, std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> _dataCache;
};

#endif // _NIGHT_MODE_TABLE_STORAGE_CACHE_H_
