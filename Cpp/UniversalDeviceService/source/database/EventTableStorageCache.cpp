#include "EventTableStorageCache.h"

#include <sstream>

#include "DbExtension.h"

EventTableStorageCache::EventTableStorageCache(IQueryExecutor* queryExecutor) :
    BaseStorageCache(queryExecutor)
{

}

StorageCacheProblem EventTableStorageCache::Select(const SelectInput& what, SelectOutput& result)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const EventTableSelectInput& customWhat = dynamic_cast<const EventTableSelectInput&>(what);
    EventTableSelectOutput& customResult = dynamic_cast<EventTableSelectOutput&>(result);

    auto iter = _dataCache.find(customWhat._description);
    if (iter != _dataCache.end())
    {
        customResult._data = iter->second;
        return { StorageCacheProblemType::NoProblems, {} };
    }

    std::stringstream queryStream;
    queryStream
        << "SELECT event FROM Events WHERE providerId = '"
        << customWhat._description._id.data()
        << "' AND providerType = '"
        << customWhat._description._type
        << "' AND active = 1";
    queryStream.flush();   
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data))
    {
        std::vector<std::string> eventStrings;
        for (const auto& row : data)
        {
            auto eventString = DbExtension::FindValueByName(row, "event");
            if (eventString.size())
                eventStrings.push_back(eventString);
        }
        _dataCache.insert(std::make_pair(customWhat._description, eventStrings));
        customResult._data = eventStrings;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem EventTableStorageCache::SelectAll(SelectAllOutput& result)
{
    std::lock_guard<std::mutex> lock(_mutex);

    EventTableSelectAllOutput& customResult = dynamic_cast<EventTableSelectAllOutput&>(result);

    _dataCache.clear();

    std::stringstream queryStream;
    queryStream
        << "SELECT providerId, providerType, event FROM Events";
    queryStream.flush();   
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data))
    {
        std::vector<std::string> eventStrings;
        for (auto& row : data)
        {
            auto providerId = DbExtension::FindValueByName(row, "providerId");
            auto providerType = DbExtension::FindValueByName(row, "providerType");
            auto eventString = DbExtension::FindValueByName(row, "event");
            if (eventString.size())
                eventStrings.push_back(eventString);
            AddToCache(providerId, providerType, eventString);
        }
        customResult._data = eventStrings;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem EventTableStorageCache::Add(AddInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const EventTableAddInput& customWhat = dynamic_cast<const EventTableAddInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "INSERT INTO Events (id, active, providerId, providerType, event) VALUES ('"
        << customWhat._id
        << "', "
        << (customWhat._active ? "1" : "0")
        << ", '"
        << customWhat._providerDescription._id.data()
        << "', '"
        << customWhat._providerDescription._type
        << "', '"
        << customWhat._event
        << "')";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
    {
        AddToCache(customWhat._providerDescription, customWhat._event);
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem EventTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what)
{
    throw std::logic_error("Invalid function call");
    return { StorageCacheProblemType::Empty, "Invalid function call" };
}

StorageCacheProblem EventTableStorageCache::Update(const UpdateInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableUpdateInput& customWhat = dynamic_cast<const EventTableUpdateInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "UPDATE Events SET active = "
        << (customWhat._active ? "1" : "0")
        << ", providerId = '"
        << customWhat._providerDescription._id.data()
        << "', providerType = '"
        << customWhat._providerDescription._type
        << "', event = '"
        << customWhat._event
        << "' WHERE id = '"
        << customWhat._id
        << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem EventTableStorageCache::Delete(const DeleteInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _dataCache.clear();

    const EventTableDeleteInput& customWhat = dynamic_cast<const EventTableDeleteInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "DELETE FROM Events WHERE id='"
        << customWhat._id
        << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

void EventTableStorageCache::AddToCache(const std::string& providerId, const std::string& providerType, const std::string& eventString)
{
    ComponentDescription description;
    description._id = Uuid(providerId);
    description._type = providerType;
    AddToCache(description, eventString);
}

void EventTableStorageCache::AddToCache(const ComponentDescription& description, const std::string& eventString)
{
    if (_dataCache.count(description) == 0)
    {
        std::vector<std::string> events = { eventString };
        _dataCache.insert(std::make_pair(description, events));
    }
    else
        _dataCache[description].push_back(eventString);
}
