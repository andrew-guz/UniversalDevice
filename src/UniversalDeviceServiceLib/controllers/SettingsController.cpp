#include "SettingsController.hpp"

#include <mutex>
#include <optional>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "Cache.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Settings.hpp"
#include "Uuid.hpp"

SettingsController::SettingsController(IQueryExecutor* queryExecutor) :
    Controller(queryExecutor) {}

std::optional<Settings> SettingsController::Get(const Uuid& id) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Settings> cacheResult = _cache.Get(id);
    if (cacheResult.has_value())
        return cacheResult;

    // Maybe not in cache but in database - we should check
    const std::string query = fmt::format("SELECT settings FROM Settings WHERE id = '{}'", id.data());
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        if (data.size() == 0)
            LOG_DEBUG_MSG(fmt::format("No settings for device {}", id.data()));
        else if (data.size() == 1) {
            if (data[0][1].empty())
                LOG_INFO_MSG(fmt::format("Empty settings for device {}", id.data()));
            else {
                const Settings settings = nlohmann::json::parse(data[0][1]).get<Settings>();
                _cache.Add(id, settings);
                return settings;
            }
        } else
            LOG_ERROR_MSG(fmt::format("Too many settings for device {}", id.data()));
    } else
        LOG_SQL_ERROR(query);

    return std::nullopt;
}

bool SettingsController::AddOrUpdate(const Uuid& id, const Settings& settings) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query =
        fmt::format("INSERT OR REPLACE INTO Settings (id, settings) VALUES ('{}', '{}')", id.data(), static_cast<nlohmann::json>(settings).dump());

    if (_queryExecutor->Execute(query)) {
        _cache.AddOrUpdate(id, settings);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

bool SettingsController::Remove(const Uuid& id) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("DELETE FROM Settings WHERE id='{}'", id.data());
    if (_queryExecutor->Execute(query)) {
        _cache.Remove(id);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}
