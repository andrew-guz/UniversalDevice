#ifndef _EVENT_TABLE_STORAGE_CACHE_H_
#define _EVENT_TABLE_STORAGE_CACHE_H_

#include <map>
#include <tuple>

#include "BaseStorageCache.h"

class EventTableStorageCache final : public BaseStorageCache {
public:
    EventTableStorageCache(IQueryExecutor* queryExecutor);

    virtual ~EventTableStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) override;

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override;

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override;

    virtual StorageCacheProblem Update(const UpdateInput& what) override;

    virtual StorageCacheProblem Delete(const DeleteInput& what) override;

private:
    std::map<std::tuple<std::string, std::string>, std::vector<std::string>> _dataCache;
};

#endif // _EVENT_TABLE_STORAGE_CACHE_H_
