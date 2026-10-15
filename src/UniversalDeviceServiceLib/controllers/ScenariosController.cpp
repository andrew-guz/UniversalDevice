#include "ScenariosController.hpp"

#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "Cache.hpp"
#include "Constants.hpp"
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

ScenariosController::ScenariosController(IQueryExecutor* queryExecutor, EventsController& eventsController) :
    Controller(queryExecutor),
    _eventsController(eventsController) //
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

    if (_queryExecutor->NewInsert("INSERT INTO Scenarios (id, scenario) VALUES (?, ?)",
                                  scenario._id.data(),
                                  static_cast<nlohmann::json>(scenario).dump()) == Constants::InvalidId) {
        return false;
    }

    _cache.Add(scenario._id, scenario);

    return true;
}

bool ScenariosController::Update(const Scenario& scenario) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_cache.Size())
        FillCache();

    if (!_queryExecutor->NewUpdate(
            "UPDATE Scenarios SET scenario = ? WHERE id = ?", static_cast<nlohmann::json>(scenario).dump(), scenario._id.data())) {
        return false;
    }

    _cache.Update(scenario._id, scenario);

    return true;
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

    Transaction transaction{ _queryExecutor };
    if (!transaction.IsActive()) {
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

    if (!transaction.Commit()) {
        LOG_ERROR_MSG("Failed to update events for scenario: failed to end transaction");
        return false;
    }

    LOG_INFO_MSG(fmt::format("Scenario '{}' activated!", scenario->_name));
    return true;
}

bool ScenariosController::Remove(const Uuid& id) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (!_queryExecutor->NewDelete("DELETE FROM Scenarios WHERE id=?", id.data())) {
        return false;
    }

    _cache.Remove(id);

    return true;
}

void ScenariosController::FillCache() const {
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->NewSelect(data, "SELECT * FROM Scenarios")) {
        const std::vector<Scenario> result = DbExtension::CreateVectorFromDbStrings<Scenario>(data);
        for (const Scenario& scenario : result) {
            _cache.Add(scenario._id, scenario);
        }
    }
}
