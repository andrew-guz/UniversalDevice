#include "ScenariosController.hpp"

#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "Cache.hpp"
#include "Command.hpp"
#include "CommandsController.hpp"
#include "Controller.hpp"
#include "DbExtension.hpp"
#include "Event.hpp"
#include "EventUtils.hpp"
#include "EventsController.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"

ScenariosController::ScenariosController(IQueryExecutor* queryExecutor, EventsController& eventsController, CommandsController& commandsController) :
    Controller(queryExecutor),
    _eventsController(eventsController),
    _commandsController(commandsController) //
{
    FillCache();
}

std::vector<Scenario> ScenariosController::List() const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_cache.Size())
        FillCache();

    return _cache.List();
}

std::optional<Scenario> ScenariosController::Get(const Uuid& id) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_cache.Size())
        FillCache();

    std::optional<Scenario> cacheResult = _cache.Get(id);
    if (cacheResult.has_value())
        return cacheResult;

    LOG_DEBUG_MSG(fmt::format("No scenario with id {}", id.data()));

    return std::nullopt;
}

bool ScenariosController::Add(const Scenario& scenario) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_cache.Size())
        FillCache();

    const std::string scenarioJson = static_cast<nlohmann::json>(scenario).dump();
    const std::string query = fmt::format("INSERT INTO Scenarios (id, scenario) VALUES ('{}', '{}')", scenario._id.data(), scenarioJson);

    if (_queryExecutor->Execute(query)) {
        _cache.Add(scenario._id, scenario);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

bool ScenariosController::Update(const Scenario& scenario) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_cache.Size())
        FillCache();

    const std::string scenarioJson = static_cast<nlohmann::json>(scenario).dump();
    const std::string query = fmt::format("UPDATE Scenarios SET scenario = '{}' WHERE id = '{}'", scenarioJson, scenario._id.data());

    if (_queryExecutor->Execute(query)) {
        _cache.Update(scenario._id, scenario);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

void ScenariosController::CleanupScenario(Scenario& scenario) {
    std::vector<Event> allEvents = _eventsController.List();
    std::set<Uuid> eventIds;
    for (const Event& event : allEvents) {
        eventIds.insert(GetEventId(event));
    }

    const auto removeNotExistingEvents = [](std::set<Uuid>& ids, const std::set<Uuid>& eventIds) {
        for (auto iter = ids.begin(); iter != ids.end();) {
            if (eventIds.count(*iter) == 0)
                iter = ids.erase(iter);
            else
                ++iter;
        }
    };

    removeNotExistingEvents(scenario._activateEvent, eventIds);
    removeNotExistingEvents(scenario._deactivateEvent, eventIds);
}

bool ScenariosController::ActivateScenario(const Uuid& id) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_cache.Size())
        FillCache();

    std::optional<Scenario> scenario = _cache.Get(id);
    if (!scenario.has_value()) {
        LOG_ERROR_MSG("Scenario not found");
        return false;
    }

    CleanupScenario(scenario.value());

    if (!_queryExecutor->Begin()) {
        LOG_ERROR_MSG("Failed to update events for scenario: failed to start transaction");
        return false;
    }

    for (const Uuid& activeEventId : scenario->_activateEvent)
        if (!_eventsController.SetActivity(activeEventId, true).has_value()) {
            LOG_ERROR_MSG("Failed to set event activity");
            return false;
        }

    for (const Uuid& activeEventId : scenario->_deactivateEvent)
        if (!_eventsController.SetActivity(activeEventId, false).has_value()) {
            LOG_ERROR_MSG("Failed to set event activity");
            return false;
        }

    for (const auto& [deviceId, command] : scenario->_commands) {
        if (!_commandsController.AddOrUpdate(deviceId, command)) {
            LOG_ERROR_MSG("Failed to write command");
            return false;
        }
    }

    if (!_queryExecutor->Commit()) {
        LOG_ERROR_MSG("Failed to update events for scenario: failed to end transaction");
        return false;
    }

    LOG_INFO_MSG(fmt::format("Scenario '{}' activated!", scenario->_name));
    return true;
}

bool ScenariosController::Remove(const Uuid& id) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("DELETE FROM Scenarios WHERE id='{}'", id.data());
    if (_queryExecutor->Execute(query)) {
        _cache.Remove(id);
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

void ScenariosController::FillCache() const {
    const std::string query = "SELECT * FROM Scenarios";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        const std::vector<Scenario> result = DbExtension::CreateVectorFromDbStrings<Scenario>(data);
        for (const Scenario& scenario : result) {
            _cache.Add(scenario._id, scenario);
        }
    } else
        LOG_SQL_ERROR(query);
}
