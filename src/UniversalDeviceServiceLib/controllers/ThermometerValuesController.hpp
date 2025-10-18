#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include "Cache.hpp"
#include "Controller.hpp"
#include "DevicesController.hpp"
#include "IQueryExecutor.hpp"
#include "ThermometerValue.hpp"
#include "Uuid.hpp"

class ThermometerValuesController final : public Controller {
public:
    ThermometerValuesController(IQueryExecutor* queryExecutor, DevicesController& devicesController);

    ~ThermometerValuesController() = default;

    bool Add(const Uuid& id, const ThermometerValue& value);

    std::vector<ThermometerValue> Get(const Uuid& id, std::uint64_t seconds = 0);

private:
    void FillCache();

private:
    DevicesController& _devicesController;
    using ThermometerValuesCache =
        Cache<std::chrono::system_clock::time_point, ThermometerValue, std::map<std::chrono::system_clock::time_point, ThermometerValue>>;
    mutable std::unordered_map<Uuid, ThermometerValuesCache> _caches;
};
