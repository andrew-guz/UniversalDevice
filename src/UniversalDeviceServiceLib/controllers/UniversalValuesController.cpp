#include "UniversalValuesController.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iterator>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Cache.hpp"
#include "DbExtension.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "TimeHelper.hpp"
#include "TimeValuesController.hpp"
#include "UniversalData.hpp"
#include "UniversalValue.hpp"

UniversalValuesController::UniversalValuesController(IQueryExecutor* queryExecutor) :
    TimeValuesController<UniversalValue>(queryExecutor) //
{
    FillCache();
}

bool UniversalValuesController::Add(const Uuid& id, const UniversalValue& value) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!value._timestamp.has_value()) {
        LOG_ERROR_MSG("Invalid UniversalValue");
        return false;
    }

    const std::string query = fmt::format("INSERT INTO UniversalDevices (id, timestamp, 'values') VALUES ('{}', {}, '{}')",
                                          id.data(),
                                          TimeHelper::TimeToInt(value._timestamp.value()),
                                          static_cast<nlohmann::json>(value._values).dump());
    if (_queryExecutor->Execute(query)) {
        _caches[id].Add(value._timestamp.value(), value);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

std::vector<UniversalValue> UniversalValuesController::Get(const Uuid& id, const std::uint64_t seconds) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_caches.contains(id) || _caches[id].Size() == 0) {
        return {};
    }

    std::vector<UniversalValue> result;
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

void UniversalValuesController::FillCache() {
    const std::string query = "SELECT * FROM UniversalDevices";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        for (const auto& dbString : data) {
            if (dbString.size() % 2 == 0) {
                auto deviceId = DbExtension::FindValueByName<Uuid>(dbString, "id");
                auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbString, "timestamp");
                auto values = DbExtension::FindValueByName<std::string>(dbString, "values");
                if (deviceId.has_value() && timestamp.has_value() && values.has_value()) {
                    const nlohmann::json valuesJson = nlohmann::json::parse(values.value());
                    if (!_caches.contains(deviceId.value()))
                        _caches.try_emplace(deviceId.value(), ChronoCache<UniversalValue>{});
                    _caches[deviceId.value()].Add(timestamp.value(),
                                                  UniversalValue{
                                                      ._values = valuesJson.get<std::map<std::string, UniversalData>>(),
                                                      ._timestamp = timestamp.value(),
                                                  });
                }
            } else
                LOG_ERROR_MSG("Invalid db strings");
        }
    } else
        LOG_SQL_ERROR(query);
}
