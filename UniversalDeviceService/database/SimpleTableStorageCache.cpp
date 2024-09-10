#include "SimpleTableStorageCache.hpp"

#include <sstream>

#include <fmt/format.h>

#include "StorageCacheFactory.hpp"

SimpleTableStorageCache::SimpleTableStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName) :
    BaseStorageCache(queryExecutor), _tableName(tableName), _fieldName(fieldName) {}

StorageCacheProblem SimpleTableStorageCache::Select(const SelectInput& what, SelectOutput& result) {
    std::lock_guard<std::mutex> lock(_mutex);

    const SimpleTableSelectInput& customWhat = dynamic_cast<const SimpleTableSelectInput&>(what);
    SimpleTableSelectOutput& customResult = dynamic_cast<SimpleTableSelectOutput&>(result);

    auto iter = _dataCache.find(customWhat._id);
    if (iter != _dataCache.end()) {
        customResult._data = iter->second;
        return { StorageCacheProblemType::NoProblems, {} };
    }

    const std::string query = fmt::format("SELECT {} FROM {} WHERE id = '{}'", _fieldName, _tableName, customWhat._id);
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        if (data.size() == 0)
            return { StorageCacheProblemType::NotExists, {} };
        else if (data.size() == 1) {
            auto dataString = data[0][1];
            if (!dataString.empty()) {
                customResult._data = dataString;
                _dataCache.insert(std::make_pair(customWhat._id, dataString));
                return { StorageCacheProblemType::NoProblems, {} };
            }
            return { StorageCacheProblemType::Empty, {} };
        } else
            return { StorageCacheProblemType::TooMany, {} };
    }
    return { StorageCacheProblemType::SQLError, query };
}

StorageCacheProblem SimpleTableStorageCache::SelectAll(SelectAllOutput& result) {
    throw std::logic_error("Invalid function call");
    return { StorageCacheProblemType::Empty, "Invalid function call" };
}

StorageCacheProblem SimpleTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    const SimpleTableInsertOrReplaceInput& customWhat = dynamic_cast<const SimpleTableInsertOrReplaceInput&>(what);

    auto iter = _dataCache.find(customWhat._id);
    if (iter != _dataCache.end())
        _dataCache.erase(iter);

    if (customWhat._data.empty())
        return { StorageCacheProblemType::Empty, {} };

    const std::string query =
        fmt::format("INSERT OR REPLACE INTO {} (id, {}) VALUES ('{}', '{}')", _tableName, _fieldName, customWhat._id, customWhat._data);
    if (_queryExecutor->Execute(query)) {
        _dataCache.insert(std::make_pair(customWhat._id, customWhat._data));
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, query };
}

StorageCacheProblem SimpleTableStorageCache::Update(const UpdateInput& what) {
    throw std::logic_error("Invalid function call");
    return { StorageCacheProblemType::Empty, "Invalid function call" };
}

StorageCacheProblem SimpleTableStorageCache::Delete(const DeleteInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const SimpleTableDeleteInput& customWhat = dynamic_cast<const SimpleTableDeleteInput&>(what);

    const std::string query = fmt::format("DELETE FROM {} WHERE id='{}'", _tableName, customWhat._id);
    if (_queryExecutor->Execute(query))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, query };
}

IStorageCache* SimpleTableStorageCache::GetSettingsCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(queryExecutor, "Settings", "settings");
}

IStorageCache* SimpleTableStorageCache::GetCommandsCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(queryExecutor, "Commands", "commands");
}
