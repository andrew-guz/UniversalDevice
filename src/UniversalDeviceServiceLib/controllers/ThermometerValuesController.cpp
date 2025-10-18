#include "ThermometerValuesController.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iterator>

#include "Controller.hpp"
#include "DbExtension.hpp"
#include "DevicesController.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "ThermometerValue.hpp"
#include "TimeHelper.hpp"

ThermometerValuesController::ThermometerValuesController(IQueryExecutor* queryExecutor, DevicesController& devicesController) :
    Controller(queryExecutor),
    _devicesController(devicesController) //
{
    FillCache();
}

bool ThermometerValuesController::Add(const Uuid& id, const ThermometerValue& value) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!value._timestamp.has_value()) {
        LOG_ERROR_MSG("Invalid ThermometerValue");
        return false;
    }

    if (std::abs(value._value - ThermometerValue::InvalidTemperature) < 0.1f) {
        const std::optional<Device> device = _devicesController.Get(id);
        const std::string deviceName = device.has_value() ? fmt::format("{} ({})", device->_name, device->_group) : "Unknown";
        LOG_INFO_MSG(fmt::format("-127.0 found - no sensor connected to '{}' ({})", deviceName, id.data()));
        return false;
    }

    const std::string query = fmt::format("INSERT INTO Thermometers (id, timestamp, value) VALUES ('{}', {}, '{}')",
                                          id.data(),
                                          TimeHelper::TimeToInt(value._timestamp.value()),
                                          value._value);
    if (_queryExecutor->Execute(query)) {
        _caches[id].Add(value._timestamp.value(), value);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

std::vector<ThermometerValue> ThermometerValuesController::Get(const Uuid& id, const std::uint64_t seconds) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_caches.contains(id) || _caches[id].Size() == 0) {
        FillCache();
    }

    if (!_caches.contains(id) || _caches[id].Size() == 0) {
        LOG_INFO_MSG(fmt::format("No data for device {} found", id.data()));
        return {};
    }

    std::vector<ThermometerValue> result;
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

void ThermometerValuesController::FillCache() {
    const std::string query = "SELECT * FROM Thermometers";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        for (const auto& dbString : data) {
            if (dbString.size() % 2 == 0) {
                auto deviceId = DbExtension::FindValueByName<Uuid>(dbString, "id");
                auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbString, "timestamp");
                auto value = DbExtension::FindValueByName<float>(dbString, "value");
                if (deviceId.has_value() && timestamp.has_value() && value.has_value()) {
                    if (!_caches.contains(deviceId.value()))
                        _caches.try_emplace(deviceId.value(), ThermometerValuesCache{});
                    _caches[deviceId.value()].Add(timestamp.value(),
                                                  ThermometerValue{
                                                      ._value = value.value(),
                                                      ._timestamp = timestamp.value(),
                                                  });
                }
            } else
                LOG_ERROR_MSG("Invalid db strings");
        }
    } else
        LOG_SQL_ERROR(query);
}
