#pragma once

#include "StorageCacheSharedData.hpp"

class IStorageCache {
public:
    virtual ~IStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) = 0;

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) = 0;

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) = 0;

    virtual StorageCacheProblem Update(const UpdateInput& what) = 0;

    virtual StorageCacheProblem Delete(const DeleteInput& what) = 0;

    virtual void Cleanup() = 0;
};
