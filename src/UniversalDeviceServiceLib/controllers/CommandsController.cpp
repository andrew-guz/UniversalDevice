#include "CommandsController.hpp"

#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Cache.hpp"
#include "Command.hpp"
#include "Controller.hpp"
#include "DbExtension.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Uuid.hpp"

CommandsController::CommandsController(IQueryExecutor* queryExecutor) :
    Controller(queryExecutor) //
{
    FillCache();
}

std::optional<Command> CommandsController::Get(const Uuid& deviceId) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Command> cacheResult = _cache.Get(deviceId);
    if (cacheResult.has_value())
        return cacheResult;

    return std::nullopt;
}

bool CommandsController::AddOrUpdate(const Uuid& deviceId, const Command& command) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format(
        "INSERT OR REPLACE INTO Commands (id, command) VALUES ('{}', '{}')", deviceId.data(), static_cast<nlohmann::json>(command).dump());

    if (_queryExecutor->Execute(query)) {
        _cache.AddOrUpdate(deviceId, command);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

bool CommandsController::Remove(const Uuid& deviceId) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Command> cacheResult = _cache.Get(deviceId);
    if (!cacheResult.has_value()) {
        LOG_DEBUG_MSG(fmt::format("No command with id {}", deviceId.data()));
        return false;
    }

    const std::string query = fmt::format("DELETE FROM Commands WHERE id='{}'", deviceId.data());
    if (_queryExecutor->Delete(query)) {
        _cache.Remove(deviceId);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

void CommandsController::FillCache() {
    const std::string query = "SELECT * FROM Commands";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        for (const std::vector<std::string>& dbStrings : data) {
            if (dbStrings.size() % 2 == 0) {
                auto deviceId = DbExtension::FindValueByName<Uuid>(dbStrings, "id");
                auto command = DbExtension::FindValueByName<std::string>(dbStrings, "command");
                if (deviceId.has_value() && command.has_value()) {
                    _cache.Add(deviceId.value(), nlohmann::json::parse(command.value()).get<Command>());
                }
            } else
                LOG_ERROR_MSG("Invalid db strings");
        }
    } else
        LOG_SQL_ERROR(query);
}
