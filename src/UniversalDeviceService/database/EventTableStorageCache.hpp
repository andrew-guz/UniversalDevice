#pragma once

#include <map>
#include <tuple>

#include "BaseStorageCache.hpp"
#include "Types.hpp"
#include "Uuid.hpp"

class EventTableStorageCache final : public BaseStorageCache {
public:
    EventTableStorageCache(IQueryExecutor* queryExecutor);

    virtual ~EventTableStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) override;

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override;

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override;

    virtual StorageCacheProblem Update(const UpdateInput& what) override;

    virtual StorageCacheProblem Delete(const DeleteInput& what) override;

    static IStorageCache* GetCache(IQueryExecutor* queryExecutor);

private:
    using Key = std::pair<Uuid, ActorType>;

    std::map<EventTableStorageCache::Key, std::vector<std::string>> _dataCache;
};
