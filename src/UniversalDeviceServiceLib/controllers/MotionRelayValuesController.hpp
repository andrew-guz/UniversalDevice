#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include "Cache.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "MotionRelayValue.hpp"
#include "Uuid.hpp"

class MotionRelayValuesController final : public Controller {
public:
    MotionRelayValuesController(IQueryExecutor* queryExecutor);

    ~MotionRelayValuesController() = default;

    bool Add(const Uuid& id, const MotionRelayValue& value);

    std::vector<MotionRelayValue> Get(const Uuid& id, std::uint64_t seconds = 0);

private:
    void FillCache();

private:
    using MotionRelayValuesCache =
        Cache<std::chrono::system_clock::time_point, MotionRelayValue, std::map<std::chrono::system_clock::time_point, MotionRelayValue>>;
    mutable std::unordered_map<Uuid, MotionRelayValuesCache> _caches;
};
