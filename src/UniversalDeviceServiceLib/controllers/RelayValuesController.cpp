#include "RelayValuesController.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iterator>

#include "Controller.hpp"
#include "DbExtension.hpp"
#include "DevicesController.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "RelayValue.hpp"
#include "TimeHelper.hpp"

RelayValuesController::RelayValuesController(IQueryExecutor* queryExecutor) :
    Controller(queryExecutor) //
{
    FillCache();
}

bool RelayValuesController::Add(const Uuid& id, const RelayValue& value) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!value._timestamp.has_value()) {
        LOG_ERROR_MSG("Invalid RelayValue");
        return false;
    }

    const std::string query = fmt::format("INSERT INTO Relays (id, timestamp, state) VALUES ('{}', {}, '{}')",
                                          id.data(),
                                          TimeHelper::TimeToInt(value._timestamp.value()),
                                          value._state);
    if (_queryExecutor->Execute(query)) {
        _caches[id].Add(value._timestamp.value(), value);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

std::vector<RelayValue> RelayValuesController::Get(const Uuid& id, const std::uint64_t seconds) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_caches.contains(id) || _caches[id].Size() == 0) {
        FillCache();
    }

    if (!_caches.contains(id) || _caches[id].Size() == 0) {
        LOG_INFO_MSG(fmt::format("No data for device {} found", id.data()));
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
    const std::string query = "SELECT * FROM Relays";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        for (const auto& dbString : data) {
            if (dbString.size() % 2 == 0) {
                auto deviceId = DbExtension::FindValueByName<Uuid>(dbString, "id");
                auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbString, "timestamp");
                auto state = DbExtension::FindValueByName<int>(dbString, "state");
                if (deviceId.has_value() && timestamp.has_value() && state.has_value()) {
                    if (!_caches.contains(deviceId.value()))
                        _caches.try_emplace(deviceId.value(), RelayValuesCache{});
                    _caches[deviceId.value()].Add(timestamp.value(),
                                                  RelayValue{
                                                      ._state = state.value(),
                                                      ._timestamp = timestamp.value(),
                                                  });
                }
            } else
                LOG_ERROR_MSG("Invalid db strings");
        }
    } else
        LOG_SQL_ERROR(query);
}
