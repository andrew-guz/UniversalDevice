#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include "Cache.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "RelayValue.hpp"
#include "Uuid.hpp"

class RelayValuesController final : public Controller {
public:
    RelayValuesController(IQueryExecutor* queryExecutor);

    ~RelayValuesController() = default;

    bool Add(const Uuid& id, const RelayValue& value);

    std::vector<RelayValue> Get(const Uuid& id, std::uint64_t seconds = 0);

private:
    void FillCache();

private:
    using RelayValuesCache = Cache<std::chrono::system_clock::time_point, RelayValue, std::map<std::chrono::system_clock::time_point, RelayValue>>;
    mutable std::unordered_map<Uuid, RelayValuesCache> _caches;
};
