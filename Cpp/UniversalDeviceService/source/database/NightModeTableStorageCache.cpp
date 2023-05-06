#include "NightModeTableStorageCache.h"

#include <sstream>

NightModeTableStorageCache::NightModeTableStorageCache(IQueryExecutor* queryExecutor) :
    BaseStorageCache(queryExecutor)
{

}

StorageCacheProblem NightModeTableStorageCache::Select(const SelectInput& what, SelectOutput& result)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const NightModeTableSelectInput& customWhat = dynamic_cast<const NightModeTableSelectInput&>(what);
    NightModeTableSelectOuput& customResult = dynamic_cast<NightModeTableSelectOuput&>(result);

    auto iter = _dataCache.find(customWhat._description);
    if (iter != _dataCache.end())
    {
        customResult._timing = iter->second;
        return { StorageCacheProblemType::NoProblems, {} };
    }

    std::stringstream queryStream;
    queryStream
        << "SELECT start, end FROM NightModeDevices WHERE id = '"
        << customWhat._description._id.data()
        << "' AND type = '"
        << customWhat._description._type
        << "'";
    queryStream.flush();   
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data) &&
        data.size() > 0 &&
        data[0].size() == 2)
    {
        NightModeTiming timing;
        timing._start = TimeHelper::TimeFromInt((int64_t)std::stoll(data[0][0]));
        timing._end = TimeHelper::TimeFromInt((int64_t)std::stoll(data[0][1]));
        _dataCache.insert(std::make_pair(customWhat._description, timing));
        customResult._timing = timing;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem NightModeTableStorageCache::SelectAll(SelectAllOutput& result)
{
    std::lock_guard<std::mutex> lock(_mutex);

    NightModeTableSelectAllOuput& customResult = dynamic_cast<NightModeTableSelectAllOuput&>(result);

    std::stringstream queryStream;
    queryStream
        << "SELECT * FROM NightModeDevices";
    queryStream.flush();   
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data))
    {
        auto devices = DbExtension::CreateVectorFromDbStrings<NightModeDevice>(data);
        _dataCache.clear();
        for (auto& device : devices)
            _dataCache.insert(std::make_pair(device._description, device._timing));
        customResult._devices = devices;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem NightModeTableStorageCache::Add(AddInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);
}

StorageCacheProblem NightModeTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    /*_dataCache.clear();

    const EventTableInsertOrReplaceInput& customWhat = dynamic_cast<const EventTableInsertOrReplaceInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "INSERT INTO Events (id, active, providerId, providerType, event) VALUES ('"
        << customWhat._id
        << "', "
        << (customWhat._active ? "1" : "0")
        << ", '"
        << customWhat._providerId.data()
        << "', '"
        << customWhat._providerType
        << "', '"
        << customWhat._event
        << "')";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, queryStream.str() };*/
}

StorageCacheProblem NightModeTableStorageCache::Update(const UpdateInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    /*_dataCache.clear();

    const EventTableUpdateInput& customWhat = dynamic_cast<const EventTableUpdateInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "UPDATE Events SET active = "
        << (customWhat._active ? "1" : "0")
        << ", providerId = '"
        << customWhat._providerId
        << "', providerType = '"
        << customWhat._providerType
        << "', event = '"
        << customWhat._event
        << "' WHERE id = '"
        << customWhat._id
        << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, queryStream.str() };*/
}

StorageCacheProblem NightModeTableStorageCache::Delete(const DeleteInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    /*_dataCache.clear();

    const EventTableDeleteInput& customWhat = dynamic_cast<const EventTableDeleteInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "DELETE FROM Events WHERE id='"
        << customWhat._id
        << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
        return { StorageCacheProblemType::NoProblems, {} };
    return { StorageCacheProblemType::SQLError, queryStream.str() };*/
}
