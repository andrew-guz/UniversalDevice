#include "RelayValuesController.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "Cache.hpp"
#include "Constants.hpp"
#include "DbExtension.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "RelayValue.hpp"
#include "TimeHelper.hpp"
#include "TimeValuesController.hpp"
#include "Uuid.hpp"

RelayValuesController::RelayValuesController(IQueryExecutor* queryExecutor) :
    TimeValuesController<RelayValue>(queryExecutor) //
{
    FillCache();
}

bool RelayValuesController::Add(const Uuid& id, const RelayValue& value) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!value._timestamp.has_value()) {
        LOG_ERROR_MSG("Invalid RelayValue");
        return false;
    }

    if (_queryExecutor->NewInsert(
            "INSERT INTO Relays (id, timestamp, state) VALUES (?, ?, ?)", id.data(), TimeHelper::TimeToInt(value._timestamp.value()), value._state) ==
        Constants::InvalidId) {
        return false;
    }

    _caches[id].Add(value._timestamp.value(), value);

    return true;
}

std::vector<RelayValue> RelayValuesController::Get(const Uuid& id, const std::uint64_t seconds) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_caches.contains(id) || _caches[id].Size() == 0) {
        return {};
    }

    std::vector<RelayValue> result;
    result.reserve(_caches[id].Size());

    if (seconds != 0) {
        const std::chrono::system_clock::time_point timeLimit = std::chrono::system_clock::now() - std::chrono::seconds(seconds);
        const auto& values = _caches[id].Get();
        std::transform(values.upper_bound(timeLimit), values.end(), std::back_inserter(result), [](const auto& pair) { return pair.second; });
        std::reverse(result.begin(), result.end());
    }

    if (result.size() == 0) {
        result.push_back(_caches[id].Get().rbegin()->second);
    }

    return result;
}

void RelayValuesController::FillCache() {
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->NewSelect(data, "SELECT * FROM Relays")) {
        for (const auto& dbString : data) {
            if (dbString.size() % 2 == 0) {
                auto deviceId = DbExtension::FindValueByName<Uuid>(dbString, "id");
                auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbString, "timestamp");
                auto state = DbExtension::FindValueByName<int>(dbString, "state");
                if (deviceId.has_value() && timestamp.has_value() && state.has_value()) {
                    if (!_caches.contains(deviceId.value()))
                        _caches.try_emplace(deviceId.value(), ChronoCache<RelayValue>{});
                    _caches[deviceId.value()].Add(timestamp.value(),
                                                  RelayValue{
                                                      ._state = state.value(),
                                                      ._timestamp = timestamp.value(),
                                                  });
                }
            } else
                LOG_ERROR_MSG("Invalid db strings");
        }
    }
}
