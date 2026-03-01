#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "Cache.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "Uuid.hpp"

template<typename DataType>
class TimeValuesController : public Controller {
public:
    TimeValuesController(IQueryExecutor* queryExecutor) :
        Controller(queryExecutor) {}

    virtual ~TimeValuesController() = default;

    virtual bool Add(const Uuid& id, const DataType& value) = 0;

    virtual std::vector<DataType> Get(const Uuid& id, std::uint64_t seconds = 0) = 0;

    void Cleanup(const std::chrono::system_clock::time_point& timestamp) {
        std::lock_guard<std::mutex> lockGuard{ _mutex };
        for (auto& [_, cache] : _caches) {
            cache.Cleanup(timestamp);
        }
    }

protected:
    virtual void FillCache() = 0;

protected:
    mutable std::unordered_map<Uuid, ChronoCache<DataType>> _caches;
};
