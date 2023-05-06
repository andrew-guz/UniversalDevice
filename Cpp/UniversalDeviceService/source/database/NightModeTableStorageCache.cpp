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
    
    _dataCache.clear();

    std::stringstream queryStream;
    queryStream
        << "SELECT * FROM NightModeDevices";
    queryStream.flush();   
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data))
    {
        auto devices = DbExtension::CreateVectorFromDbStrings<NightModeDevice>(data);
        for (auto& device : devices)
            _dataCache.insert(std::make_pair(device._description, device._timing));
        customResult._devices = devices;
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem NightModeTableStorageCache::Add(AddInput& what)
{
    throw std::logic_error("Invalid function call");
    return { StorageCacheProblemType::Empty, "Invalid function call" };
}

StorageCacheProblem NightModeTableStorageCache::InsertOrReplace(const InsertOrReplaceInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const NightModeTableInsertOrReplaceInput& customWhat = dynamic_cast<const NightModeTableInsertOrReplaceInput&>(what);

    auto iter = _dataCache.find(customWhat._device._description);
    if (iter != _dataCache.end())
        _dataCache.erase(iter);

    std::stringstream queryStream;
    queryStream
        << "INSERT OR REPLACE INTO NightModeDevices (id, type, start, end) VALUES ('"
        << customWhat._device._description._id.data()
        << "', "
        << customWhat._device._description._type
        << ", "
        << TimeHelper::TimeToInt(customWhat._device._timing._start)
        << ", "
        << TimeHelper::TimeToInt(customWhat._device._timing._end)
        << ")";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
    {
        _dataCache.insert(std::make_pair(customWhat._device._description, customWhat._device._timing));
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem NightModeTableStorageCache::Update(const UpdateInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const NightModeTableUpdateInput& customWhat = dynamic_cast<const NightModeTableUpdateInput&>(what);

    auto iter = _dataCache.find(customWhat._device._description);
    if (iter != _dataCache.end())
        _dataCache.erase(iter);

    std::stringstream queryStream;
    queryStream
        << "UPDATE NightModeDevices SET start = "
        << TimeHelper::TimeToInt(customWhat._device._timing._start)
        << ", end = "
        << TimeHelper::TimeToInt(customWhat._device._timing._end)
        << " WHERE id = '"
        << customWhat._device._description._id.data()
        << "' AND type = '"
        << customWhat._device._description._type
        << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
    {
        _dataCache.insert(std::make_pair(customWhat._device._description, customWhat._device._timing));
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}

StorageCacheProblem NightModeTableStorageCache::Delete(const DeleteInput& what)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const NightModeTableDeleteInput& customWhat = dynamic_cast<const NightModeTableDeleteInput&>(what);

    std::stringstream queryStream;
    queryStream
        << "DELETE FROM Events WHERE id='"
        << customWhat._description._id.data()
        << "' AND type = '"
        << customWhat._description._type
        << "'";
    queryStream.flush();
    if (_queryExecutor->Execute(queryStream.str()))
    {
        auto iter = _dataCache.find(customWhat._description);
        if (iter != _dataCache.end())
            _dataCache.erase(iter);
        return { StorageCacheProblemType::NoProblems, {} };
    }
    return { StorageCacheProblemType::SQLError, queryStream.str() };
}
