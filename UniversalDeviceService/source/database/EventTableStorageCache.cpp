#include "EventTableStorageCache.h"

#include <sstream>

#include "DbExtension.h"

#include "StorageCacheFactory.h"

EventTableStorageCache::EventTableStorageCache(IQueryExecutor* queryExecutor) : BaseStorageCache(queryExecutor) {}

StorageCacheProblem EventTableStorageCache::Select(const SelectInput& what, SelectOutput& result) {
    std::lock_guard<std::mutex> lock(_mutex);

    const EventTableSelectInput& customWhat = dynamic_cast<const EventTableSelectInput&>(what);
    EventTableSelectOutput& customResult = dynamic_cast<EventTableSelectOutput&>(result);

    auto iter = _dataCache.find(std::make_tuple(customWhat._id, customWhat._type));
    if (iter != _dataCache.end()) {
        customResult._data = iter->second;
        return {StorageCacheProblemType::NoProblems, {}};
    }

    std::stringstream queryStream;
    queryStream << "SELECT event FROM Events WHERE providerId = '" << customWhat._id << "' AND providerType = '" << customWhat._type << "' AND active = 1";
    queryStream.flush();
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data)) {
        std::vector<std::string> eventStrings;
        for (const auto& row : data) {
            auto eventString = DbExtension::FindValueByName(row, "event");
            if (eventString.size())
                eventStrings.push_back(eventString);
        }
        _dataCache.insert(std::make_pair(std::make_tuple(customWhat._id, customWhat._type), eventStrings));
        customResult._data = eventStrings;
        return {StorageCacheProblemType::NoProblems, {}};
    }
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

StorageCacheProblem EventTableStorageCache::SelectAll(SelectAllOutput& result) {
    std::lock_guard<std::mutex> lock(_mutex);

    EventTableSelectAllOutput& customResult = dynamic_cast<EventTableSelectAllOutput&>(result);

    std::stringstream queryStream;
    queryStream << "SELECT event FROM Events";
    queryStream.flush();
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data)) {
        std::vector<std::string> eventStrings;
        for (auto& row : data) {
            auto eventString = DbExtension::FindValueByName(row, "event");
            if (eventString.size())
                eventStrings.push_back(eventString);
        }
        customResult._data = eventStrings;
        return {StorageCacheProblemType::NoProblems, {}};
    }
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

StorageCacheProblem EventTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableInsertOrReplaceInput& customWhat = dynamic_cast<const EventTableInsertOrReplaceInput&>(what);

    std::stringstream queryStream;
    queryStream << "INSERT INTO Events (id, active, providerId, providerType, event) VALUES ('" << customWhat._id << "', " << (customWhat._active ? "1" : "0") << ", '" << customWhat._providerId.data()
                << "', '" << customWhat._providerType << "', '" << customWhat._event << "')";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return {StorageCacheProblemType::NoProblems, {}};
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

StorageCacheProblem EventTableStorageCache::Update(const UpdateInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableUpdateInput& customWhat = dynamic_cast<const EventTableUpdateInput&>(what);

    std::stringstream queryStream;
    queryStream << "UPDATE Events SET active = " << (customWhat._active ? "1" : "0") << ", providerId = '" << customWhat._providerId << "', providerType = '" << customWhat._providerType
                << "', event = '" << customWhat._event << "' WHERE id = '" << customWhat._id << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return {StorageCacheProblemType::NoProblems, {}};
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

StorageCacheProblem EventTableStorageCache::Delete(const DeleteInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableDeleteInput& customWhat = dynamic_cast<const EventTableDeleteInput&>(what);

    std::stringstream queryStream;
    queryStream << "DELETE FROM Events WHERE id='" << customWhat._id << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return {StorageCacheProblemType::NoProblems, {}};
    return {StorageCacheProblemType::SQLError, queryStream.str()};
}

IStorageCache* EventTableStorageCache::GetCache(IQueryExecutor* queryExecutor) { return StorageCacheFactory::Instance()->GetStorageCache<EventTableStorageCache, false>(queryExecutor, "Events"); }
