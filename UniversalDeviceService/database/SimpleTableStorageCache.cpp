#include "SimpleTableStorageCache.hpp"

#include <sstream>

#include "StorageCacheFactory.hpp"

SimpleTableStorageCache::SimpleTableStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName)
    : BaseStorageCache(queryExecutor), _tableName(tableName), _fieldName(fieldName) {}

StorageCacheProblem SimpleTableStorageCache::Select(const SelectInput& what, SelectOutput& result) {
    std::lock_guard<std::mutex> lock(_mutex);

    const SimpleTableSelectInput& customWhat = dynamic_cast<const SimpleTableSelectInput&>(what);
    SimpleTableSelectOutput& customResult = dynamic_cast<SimpleTableSelectOutput&>(result);

    auto iter = _dataCache.find(customWhat._id);
    if (iter != _dataCache.end()) {
        customResult._data = iter->second;
        return {StorageCacheProblemType::NoProblems, {}};
    }

    std::stringstream queryStream;
    queryStream << "SELECT " << _fieldName << " FROM " << _tableName << " WHERE id = '" << customWhat._id << "'";
    queryStream.flush();
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data)) {
        if (data.size() == 0)
            return {StorageCacheProblemType::NotExists, {}};
        else if (data.size() == 1) {
            auto dataString = data[0][1];
            if (!dataString.empty()) {
                customResult._data = dataString;
                _dataCache.insert(std::make_pair(customWhat._id, dataString));
                return {StorageCacheProblemType::NoProblems, {}};
            }
            return {StorageCacheProblemType::Empty, {}};
        } else
            return {StorageCacheProblemType::TooMany, {}};
    }
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

StorageCacheProblem SimpleTableStorageCache::SelectAll(SelectAllOutput& result) {
    throw std::logic_error("Invalid function call");
    return {StorageCacheProblemType::Empty, "Invalid function call"};
}

StorageCacheProblem SimpleTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    const SimpleTableInsertOrReplaceInput& customWhat = dynamic_cast<const SimpleTableInsertOrReplaceInput&>(what);

    auto iter = _dataCache.find(customWhat._id);
    if (iter != _dataCache.end())
        _dataCache.erase(iter);

    if (!customWhat._data.empty()) {
        std::stringstream queryStream;
        queryStream << "INSERT OR REPLACE INTO " << _tableName << " (id, " << _fieldName << ") VALUES ('" << customWhat._id << "', '"
                    << customWhat._data << "')";
        queryStream.flush();
        if (_queryExecutor->Execute(queryStream.str())) {
            _dataCache.insert(std::make_pair(customWhat._id, customWhat._data));
            return {StorageCacheProblemType::NoProblems, {}};
        }
        return {StorageCacheProblemType::SQLError, queryStream.str()};
    }
    return {StorageCacheProblemType::Empty, {}};
}

StorageCacheProblem SimpleTableStorageCache::Update(const UpdateInput& what) {
    throw std::logic_error("Invalid function call");
    return {StorageCacheProblemType::Empty, "Invalid function call"};
}

StorageCacheProblem SimpleTableStorageCache::Delete(const DeleteInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const SimpleTableDeleteInput& customWhat = dynamic_cast<const SimpleTableDeleteInput&>(what);

    std::stringstream queryStream;
    queryStream << "DELETE FROM " << _tableName << " WHERE id='" << customWhat._id << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return {StorageCacheProblemType::NoProblems, {}};
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

IStorageCache* SimpleTableStorageCache::GetSettingsCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(queryExecutor, "Settings", "settings");
}

IStorageCache* SimpleTableStorageCache::GetCommandsCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(queryExecutor, "Commands", "commands");
}
