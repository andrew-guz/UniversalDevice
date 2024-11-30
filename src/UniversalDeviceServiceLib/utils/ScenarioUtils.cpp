#include <nlohmann/json.hpp>

#include "Event.hpp"
#include "EventTableStorageCache.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Scenario.hpp"
#include "SimpleTableStorageCache.hpp"

void CleanupScenario(Scenario& scenario, IQueryExecutor* queryExecutor) {
    IStorageCache* eventsStorageCache = EventTableStorageCache::GetCache(queryExecutor);
    EventTableSelectAllOutput allEvents;
    const StorageCacheProblem selectAllEventsProblem = eventsStorageCache->SelectAll(allEvents);
    if (selectAllEventsProblem._type != StorageCacheProblemType::NoProblems)
        return;
    std::set<Uuid> eventIds;
    for (const std::string& eventString : allEvents._data) {
        eventIds.insert(nlohmann::json::parse(eventString).at("id").get<Uuid>());
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
    removeNotExistingEvents(scenario._activateEvent, eventIds);
}

bool ActivateScenario(const Scenario& scenario, IQueryExecutor* queryExecutor) {
    IStorageCache* eventsStorageCache = EventTableStorageCache::GetCache(queryExecutor);
    EventTableSelectAllOutput allEventsOutput;
    const StorageCacheProblem selectAllEventsProblem = eventsStorageCache->SelectAll(allEventsOutput);
    if (selectAllEventsProblem._type != StorageCacheProblemType::NoProblems)
        return false;
    std::map<Uuid, std::pair<Event, nlohmann::json>> events;
    for (const std::string& eventString : allEventsOutput._data) {
        const nlohmann::json eventJson = nlohmann::json::parse(eventString);
        const Event event = eventJson.get<Event>();
        events.try_emplace(event._id, std::make_pair(event, eventJson));
    }
    std::map<Uuid, std::pair<Event, nlohmann::json>> modifiedEvents;
    for (auto& [eventId, eventPair] : events) {
        auto& [event, eventJson] = eventPair;
        if (scenario._activateEvent.contains(eventId)) {
            event._active = true;
            eventJson["active"] = true;
            modifiedEvents.try_emplace(eventId, std::make_pair(event, eventJson));
        }
        if (scenario._deactivateEvent.contains(eventId)) {
            event._active = false;
            eventJson["active"] = false;
            modifiedEvents.try_emplace(eventId, std::make_pair(event, eventJson));
        }
    }
    if (!queryExecutor->Begin()) {
        LOG_ERROR_MSG("Failed to update events for scenario: failed to start transaction");
        return false;
    }
    for (const auto& [eventId, eventPair] : modifiedEvents) {
        auto& [event, eventJson] = eventPair;
        const StorageCacheProblem problem = eventsStorageCache->Update(EventTableUpdateInput{ ._id = eventId,
                                                                                              ._active = event._active,
                                                                                              ._providerId = event._provider._id,
                                                                                              ._providerType = event._provider._type,
                                                                                              ._event = eventJson.dump() });
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                break;
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                LOG_ERROR_MSG(fmt::format("Failed to update events for scenario: {}", eventJson.dump()));
                return false;
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                return false;
                break;
        }
    }
    if (!queryExecutor->Commit()) {
        LOG_ERROR_MSG("Failed to update events for scenario: failed to end transaction");
        return false;
    }
    LOG_INFO_MSG(fmt::format("Scenario '{}' activated!", scenario._name));
    return true;
}

bool ActivateScenario(IQueryExecutor* queryExecutor, const Uuid& scenarioId) {
    try {
        IStorageCache* scenariosStorageCache = GetScenariosCache(queryExecutor);
        SimpleTableSelectOutput<Scenario> scenarioOutput;
        const StorageCacheProblem problem = scenariosStorageCache->Select(
            SimpleTableSelectInput{
                ._id = Uuid(scenarioId),
            },
            scenarioOutput);

        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                if (ActivateScenario(scenarioOutput._data, queryExecutor))
                    return true;
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::ActivateScenario");
    }
    return false;
}
