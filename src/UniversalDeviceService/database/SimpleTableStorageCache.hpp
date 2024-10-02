#pragma once

#include <map>

#include "BaseStorageCache.hpp"
#include "Uuid.hpp"

class SimpleTableStorageCache final : public BaseStorageCache {
public:
    SimpleTableStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName);

    virtual ~SimpleTableStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) override;

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override;

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override;

    virtual StorageCacheProblem Update(const UpdateInput& what) override;

    virtual StorageCacheProblem Delete(const DeleteInput& what) override;

    static IStorageCache* GetSettingsCache(IQueryExecutor* queryExecutor);

    static IStorageCache* GetCommandsCache(IQueryExecutor* queryExecutor);

private:
    std::string _tableName;
    std::string _fieldName;
    std::map<Uuid, std::string> _dataCache;
};
