#ifndef _EVENT_TABLE_STORAGE_CACHE_H_
#define _EVENT_TABLE_STORAGE_CACHE_H_

#include <tuple>
#include <map>

#include "BaseStorageCache.h"

class EventTableStorageCache final : public BaseStorageCache
{
public:
    EventTableStorageCache(IQueryExecutor* queryExecutor);

    virtual ~EventTableStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) override;

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override;

    virtual StorageCacheProblem Add(AddInput& what) override;

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override;

    virtual StorageCacheProblem Update(const UpdateInput& what) override;

    virtual StorageCacheProblem Delete(const DeleteInput& what) override;

private:
    void AddToCache(const std::string& providerId, const std::string& providerType, const std::string& eventString);

    void AddToCache(const ComponentDescription& description, const std::string& eventString);

private:
    std::map<ComponentDescription, std::vector<std::string>> _dataCache;
};

#endif // _EVENT_TABLE_STORAGE_CACHE_H_
