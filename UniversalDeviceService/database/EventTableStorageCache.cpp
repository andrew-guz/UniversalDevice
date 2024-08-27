#include "EventTableStorageCache.hpp"

#include <utility>

#include <fmt/format.h>

#include "DbExtension.hpp"
#include "Marshaling.hpp"
#include "StorageCacheFactory.hpp"

EventTableStorageCache::EventTableStorageCache(IQueryExecutor* queryExecutor) : BaseStorageCache(queryExecutor) {}

StorageCacheProblem EventTableStorageCache::Select(const SelectInput& what, SelectOutput& result) {
    std::lock_guard<std::mutex> lock(_mutex);

    const EventTableSelectInput& customWhat = dynamic_cast<const EventTableSelectInput&>(what);
    EventTableSelectOutput& customResult = dynamic_cast<EventTableSelectOutput&>(result);

    auto iter = _dataCache.find(std::make_pair(customWhat._id, customWhat._type));
    if (iter != _dataCache.end()) {
        customResult._data = iter->second;
        return { StorageCacheProblemType::NoProblems, {} };
    }

    const std::string query = fmt::format("SELECT event FROM Events WHERE providerId = '{}' AND providerType = '{}' AND active = 1", customWhat._id,
                                          ActorTypeToString(customWhat._type));
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        std::vector<std::string> eventStrings;
        for (const auto& row : data) {
            auto eventString = DbExtension::FindValueByName<std::string>(row, "event");
            if (eventString.has_value())
                eventStrings.push_back(eventString.value());
        }
        _dataCache.insert(std::make_pair(std::make_pair(customWhat._id, customWhat._type), eventStrings));
        customResult._data = eventStrings;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, query };
}

StorageCacheProblem EventTableStorageCache::SelectAll(SelectAllOutput& result) {
    std::lock_guard<std::mutex> lock(_mutex);

    EventTableSelectAllOutput& customResult = dynamic_cast<EventTableSelectAllOutput&>(result);

    static const std::string query = "SELECT event FROM Events";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        std::vector<std::string> eventStrings;
        for (auto& row : data) {
            auto eventString = DbExtension::FindValueByName<std::string>(row, "event");
            if (eventString.has_value())
                eventStrings.push_back(eventString.value());
        }
        customResult._data = eventStrings;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, query };
}

StorageCacheProblem EventTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableInsertOrReplaceInput& customWhat = dynamic_cast<const EventTableInsertOrReplaceInput&>(what);

    const std::string query =
        fmt::format("INSERT INTO Events (id, active, providerId, providerType, event) VALUES ('{}', {}, '{}', '{}', '{}')", customWhat._id,
                    (customWhat._active ? "1" : "0"), customWhat._providerId.data(), ActorTypeToString(customWhat._providerType), customWhat._event);
    if (_queryExecutor->Execute(query))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, query };
}

StorageCacheProblem EventTableStorageCache::Update(const UpdateInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableUpdateInput& customWhat = dynamic_cast<const EventTableUpdateInput&>(what);

    const std::string query = fmt::format("UPDATE Events SET active = {}, providerId = '{}', providerType = '{}', event = '{}' WHERE id = '{}'",
                                          (customWhat._active ? "1" : "0"), customWhat._providerId, ActorTypeToString(customWhat._providerType),
                                          customWhat._event, customWhat._id);
    if (_queryExecutor->Execute(query))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, query };
}

StorageCacheProblem EventTableStorageCache::Delete(const DeleteInput& what) {
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableDeleteInput& customWhat = dynamic_cast<const EventTableDeleteInput&>(what);

    const std::string query = fmt::format("DELETE FROM Events WHERE id='{}'", customWhat._id);
    if (_queryExecutor->Execute(query))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, query };
}

IStorageCache* EventTableStorageCache::GetCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<EventTableStorageCache, false>(queryExecutor, "Events");
}
