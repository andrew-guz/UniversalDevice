#pragma once

#include <map>
#include <string>
#include <type_traits>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "BaseStorageCache.hpp"
#include "DbExtension.hpp"
#include "Marshaling.hpp"
#include "StorageCacheSharedData.hpp"
#include "Uuid.hpp"

template<typename T>
class SimpleTableStorageCache : public BaseStorageCache {
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
                    T object;
                    if constexpr (std::is_same_v<T, std::string>) {
                        object = dataString;
                    } else {
                        T object = nlohmann::json::parse(dataString).get<T>();
                    }
                    customResult._data = object;
                    _dataCache.insert(std::make_pair(customWhat._id, object));
                    return { StorageCacheProblemType::NoProblems, {} };
                }
                return { StorageCacheProblemType::Empty, {} };
            } else
                return { StorageCacheProblemType::TooMany, {} };
        }
        return { StorageCacheProblemType::SQLError, query };
    }

    virtual StorageCacheProblem SelectAll(SelectAllOutput& result) override {
        const std::lock_guard<std::mutex> lock(_mutex);

        _dataCache.clear();

        SimpleTableSelectAllOutput<T>& customResult = static_cast<SimpleTableSelectAllOutput<T>&>(result);

        static const std::string query = fmt::format("SELECT id, {} FROM {}", _fieldName, _tableName);
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data)) {
            std::vector<T> resultData;
            for (auto& row : data) {
                const auto id = DbExtension::FindValueByName<Uuid>(row, "id");
                const auto dataString = DbExtension::FindValueByName<std::string>(row, _fieldName);
                if (id.has_value() && dataString.has_value()) {
                    T object;
                    if constexpr (std::is_same_v<T, std::string>) {
                        object = dataString.value();
                    } else {
                        object = nlohmann::json::parse(dataString.value()).get<T>();
                    }
                    resultData.push_back(object);
                    _dataCache.try_emplace(id.value(), object);
                }
            }
            customResult._data = resultData;
            return { StorageCacheProblemType::NoProblems, {} };
        }
        return { StorageCacheProblemType::SQLError, query };
    }

    virtual StorageCacheProblem InsertOrReplace(const InsertOrReplaceInput& what) override {
        const std::lock_guard<std::mutex> lock(_mutex);

        const SimpleTableInsertOrReplaceInput<T>& customWhat = static_cast<const SimpleTableInsertOrReplaceInput<T>&>(what);

        auto iter = _dataCache.find(customWhat._id);
        if (iter != _dataCache.end())
            _dataCache.erase(iter);

        const std::string customWhatDataString = static_cast<nlohmann::json>(customWhat._data).dump();
        if (customWhatDataString.empty())
            return { StorageCacheProblemType::Empty, {} };

        const std::string query = fmt::format(
            "INSERT OR REPLACE INTO {} (id, {}) VALUES ('{}', '{}')", _tableName, _fieldName, customWhat._id.data(), customWhatDataString);
        if (_queryExecutor->Execute(query)) {
            _dataCache.insert(std::make_pair(customWhat._id, customWhat._data));
            return { StorageCacheProblemType::NoProblems, {} };
        }
        return { StorageCacheProblemType::SQLError, query };
    }

    virtual StorageCacheProblem Update(const UpdateInput& what) override {
        const std::lock_guard<std::mutex> lock(_mutex);

        const SimpleTableUpdateInput<T>& customWhat = static_cast<const SimpleTableUpdateInput<T>&>(what);

        auto iter = _dataCache.find(customWhat._id);
        if (iter == _dataCache.end()) {
            return { StorageCacheProblemType::NotExists, {} };
        }

        const std::string query = fmt::format("UPDATE {} SET {} = '{}' WHERE id = '{}'",
                                              _tableName,
                                              _fieldName,
                                              static_cast<nlohmann::json>(customWhat._data).dump(),
                                              customWhat._id.data());
        if (_queryExecutor->Execute(query)) {
            _dataCache[customWhat._id] = customWhat._data;
            return { StorageCacheProblemType::NoProblems, {} };
        }
        return { StorageCacheProblemType::SQLError, query };
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

    virtual void Cleanup() override {
        const std::lock_guard<std::mutex> lock(_mutex);

        _dataCache.clear();
    }

private:
    std::string _tableName;
    std::string _fieldName;
    std::map<Uuid, T> _dataCache;
};

IStorageCache* GetSettingsCache(IQueryExecutor* queryExecutor);

IStorageCache* GetCommandsCache(IQueryExecutor* queryExecutor);

IStorageCache* GetScenariosCache(IQueryExecutor* queryExecutor);
