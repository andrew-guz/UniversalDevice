#include "SettingsController.hpp"

#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Cache.hpp"
#include "Constants.hpp"
#include "Controller.hpp"
#include "DbExtension.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Settings.hpp"
#include "Uuid.hpp"

SettingsController::SettingsController(IQueryExecutor* queryExecutor) :
    Controller(queryExecutor) //
{
    FillCache();
}

std::optional<Settings> SettingsController::Get(const Uuid& deviceId) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Settings> cacheResult = _cache.Get(deviceId);
    if (cacheResult.has_value())
        return cacheResult;

    return std::nullopt;
}

bool SettingsController::AddOrUpdate(const Uuid& deviceId, const Settings& settings) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_queryExecutor->NewInsert("INSERT OR REPLACE INTO Settings (id, settings) VALUES (?, ?)",
                                  deviceId.data(),
                                  static_cast<nlohmann::json>(settings).dump()) == Constants::InvalidId) {
        return false;
    }

    _cache.AddOrUpdate(deviceId, settings);

    return true;
}

bool SettingsController::Remove(const Uuid& deviceId) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Settings> cacheResult = _cache.Get(deviceId);
    if (!cacheResult.has_value()) {
        LOG_DEBUG_MSG(fmt::format("No settings with id {}", deviceId.data()));
        return false;
    }

    if (!_queryExecutor->NewDelete("DELETE FROM Settings WHERE id=?", deviceId.data())) {
        return false;
    }

    _cache.Remove(deviceId);

    return true;
}

void SettingsController::FillCache() {
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->NewSelect(data, "SELECT * FROM Settings")) {
        for (const std::vector<std::string>& dbStrings : data) {
            if (dbStrings.size() % 2 == 0) {
                auto deviceId = DbExtension::FindValueByName<Uuid>(dbStrings, "id");
                auto settings = DbExtension::FindValueByName<std::string>(dbStrings, "settings");
                if (deviceId.has_value() && settings.has_value()) {
                    _cache.Add(deviceId.value(), nlohmann::json::parse(settings.value()).get<Settings>());
                }
            } else
                LOG_ERROR_MSG("Invalid db strings");
        }
    }
}
