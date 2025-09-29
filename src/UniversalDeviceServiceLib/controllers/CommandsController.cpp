#include "CommandsController.hpp"

#include <mutex>
#include <optional>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "Cache.hpp"
#include "Command.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Uuid.hpp"

namespace {
    constexpr std::string TableName = "Commands";
    constexpr std::string ColumnName = "commands";
} // namespace

CommandsController::CommandsController(IQueryExecutor* queryExecutor) :
    Controller(queryExecutor) {}

std::optional<Command> CommandsController::Get(const Uuid& id) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Command> cacheResult = _cache.Get(id);
    if (cacheResult.has_value())
        return cacheResult;

    // Maybe not in cache but in database - we should check
    const std::string query = fmt::format("SELECT {} FROM {} WHERE id = 'id'", ColumnName, TableName, id.data());
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        if (data.size() == 0)
            LOG_DEBUG_MSG(fmt::format("No command for device {}", id.data()));
        else if (data.size() == 1) {
            if (data[0][1].empty())
                LOG_INFO_MSG(fmt::format("Empty command for device {}", id.data()));
            else {
                const Command command = nlohmann::json::parse(data[0][1]).get<Command>();
                _cache.Add(id, command);
                return command;
            }
        } else
            LOG_ERROR_MSG(fmt::format("Too many commands for device {}", id.data()));
    } else
        LOG_SQL_ERROR(query);

    return std::nullopt;
}

bool CommandsController::AddOrUpdate(const Uuid& id, const Command& command) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format(
        "INSERT OR REPLACE INTO {} (id, {}) VALUES ('{}', '{}')", TableName, ColumnName, id.data(), static_cast<nlohmann::json>(command).dump());

    if (_queryExecutor->Execute(query)) {
        _cache.AddOrUpdate(id, command);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

bool CommandsController::Remove(const Uuid& id) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("DELETE FROM {} WHERE id='{}'", TableName, id.data());
    if (_queryExecutor->Execute(query)) {
        _cache.Remove(id);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}
