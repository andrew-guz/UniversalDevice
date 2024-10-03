#pragma once

#include <map>

#include "BaseStorageCache.hpp"
#include "Uuid.hpp"

template<typename T>
class SimpleTableStorageCache final : public BaseStorageCache {
public:
    SimpleTableStorageCache(IQueryExecutor* queryExecutor, const std::string& tableName, const std::string& fieldName) :
        BaseStorageCache(queryExecutor),
        _tableName(tableName),
        _fieldName(fieldName) {}

    virtual ~SimpleTableStorageCache() = default;

    virtual StorageCacheProblem Select(const SelectInput& what, SelectOutput& result) override {
        const std::lock_guard<std::mutex> lock(_mutex);

        const SimpleTableSelectInput& customWhat = static_cast<const SimpleTableSelectInput&>(what);
        SimpleTableSelectOutput<T>& customResult = static_cast<SimpleTableSelectOutput<T>&>(result);

        auto iter = _dataCache.find(customWhat._id);
        if (iter != _dataCache.end()) {
            customResult._data = iter->second;
            return { StorageCacheProblemType::NoProblems, {} };
        }

        const std::string query = fmt::format("SELECT {} FROM {} WHERE id = '{}'", _fieldName, _tableName, customWhat._id.data());
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

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override {
        throw std::logic_error("Invalid function call");
        return { StorageCacheProblemType::Empty, "Invalid function call" };
    }

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override {
        const std::lock_guard<std::mutex> lock(_mutex);

        const SimpleTableInsertOrReplaceInput<T>& customWhat = static_cast<const SimpleTableInsertOrReplaceInput<T>&>(what);

        auto iter = _dataCache.find(customWhat._id);
        if (iter != _dataCache.end())
            _dataCache.erase(iter);

        if (customWhat._data.empty())
            return { StorageCacheProblemType::Empty, {} };

        const std::string query =
            fmt::format("INSERT OR REPLACE INTO {} (id, {}) VALUES ('{}', '{}')", _tableName, _fieldName, customWhat._id.data(), customWhat._data);
        if (_queryExecutor->Execute(query)) {
            _dataCache.insert(std::make_pair(customWhat._id, customWhat._data));
            return { StorageCacheProblemType::NoProblems, {} };
        }
        return { StorageCacheProblemType::SQLError, query };
    }

    virtual StorageCacheProblem Update(const UpdateInput& what) override {
        throw std::logic_error("Invalid function call");
        return { StorageCacheProblemType::Empty, "Invalid function call" };
    }

    virtual StorageCacheProblem Delete(const DeleteInput& what) override {
        const std::lock_guard<std::mutex> lock(_mutex);

        _dataCache.clear();

        const SimpleTableDeleteInput& customWhat = static_cast<const SimpleTableDeleteInput&>(what);

        const std::string query = fmt::format("DELETE FROM {} WHERE id='{}'", _tableName, customWhat._id.data());
        if (_queryExecutor->Execute(query))
            return { StorageCacheProblemType::NoProblems, {} };
        return { StorageCacheProblemType::SQLError, query };
    }

private:
    std::string _tableName;
    std::string _fieldName;
    std::map<Uuid, T> _dataCache;
};

IStorageCache* GetSettingsCache(IQueryExecutor* queryExecutor);

IStorageCache* GetCommandsCache(IQueryExecutor* queryExecutor);
