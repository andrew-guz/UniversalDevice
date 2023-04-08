#include "SimpleTableStorageCache.h"

#include <sstream>

SimpleTableStorageCache::SimpleTableStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName) :
    _queryExecutor(queryExecutor),
    _tableName(tableName),
    _fieldName(fieldName)
{

}

std::tuple<std::string, StorageCacheSharedData::Problem> SimpleTableStorageCache::Select(const std::string& id)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto iter = _dataCache.find(id);
    if (iter != _dataCache.end())
        return std::tuple<std::string, StorageCacheSharedData::Problem>(iter->second, { StorageCacheSharedData::ProblemType::NoProblems, {} });

    std::stringstream queryStream;
    queryStream << "SELECT "
        << _fieldName
        << " FROM "
        << _tableName
        << " WHERE id = '"
        << id
        << "'";
    queryStream.flush();
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data))
    {
        if (data.size() == 0)
            return std::make_tuple<std::string, StorageCacheSharedData::Problem>(std::string(), { StorageCacheSharedData::ProblemType::NotExists, {} });
        else if (data.size() == 1)
        {
            auto dataString = data[0][1];
            if (!dataString.empty())
            {
                _dataCache.insert(std::make_pair(id, dataString));
                return std::tuple<std::string, StorageCacheSharedData::Problem>(dataString, { StorageCacheSharedData::ProblemType::NoProblems, {} });
            }
            return std::make_tuple<std::string, StorageCacheSharedData::Problem>(std::string(), { StorageCacheSharedData::ProblemType::Empty, {} });
        }
        else 
            return std::make_tuple<std::string, StorageCacheSharedData::Problem>(std::string(), { StorageCacheSharedData::ProblemType::TooMany, {} });
    }
    return std::make_tuple<std::string, StorageCacheSharedData::Problem>({}, { StorageCacheSharedData::ProblemType::SQLError, queryStream.str() });
}

StorageCacheSharedData::Problem SimpleTableStorageCache::InsertOrReplace(const std::string& id, const std::string& data)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto iter = _dataCache.find(id);
    if (iter != _dataCache.end())
        _dataCache.erase(iter);

    if (!data.empty())
    {
        std::stringstream queryStream;
        queryStream << "INSERT OR REPLACE INTO "
            << _tableName
            << " (id, "
            << _fieldName
            << ") VALUES ('"
            << id
            << "', '"
            << data
            << "')";
        queryStream.flush();
        if (_queryExecutor->Execute(queryStream.str()))
        {
            _dataCache.insert(std::make_pair(id, data));
            return { StorageCacheSharedData::ProblemType::NoProblems, {} };
        }
        return { StorageCacheSharedData::ProblemType::SQLError, queryStream.str() };
    }
    return { StorageCacheSharedData::ProblemType::Empty, {} };
}
