#include <filesystem>
#include <stdexcept>

#include <catch2/catch_all.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Constants.hpp"
#include "Enums.hpp"
#include "EventTableStorageCache.hpp"
#include "IStorageCache.hpp"
#include "SQLiteStorage.hpp"
#include "Scenario.hpp"
#include "ScenarioUtils.hpp"
#include "SimpleTableStorageCache.hpp"
#include "StorageCacheSharedData.hpp"
#include "TimerEvent.hpp"
#include "Uuid.hpp"

TEST_CASE("Scenario activation test") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {

        SQLiteStorage storage{ dbPath };

        IStorageCache* scenarioCache = GetScenariosCache(&storage);
        IStorageCache* eventCache = EventTableStorageCache::GetCache(&storage);

        StorageCacheProblem problem;

        const auto addEvent = [&eventCache](bool active) -> Uuid {
            TimerEvent event;
            event._active = active;
            const StorageCacheProblem problem = eventCache->InsertOrReplace(EventTableInsertOrReplaceInput{
                ._id = event._id,
                ._active = event._active,
                ._providerId = Constants::PredefinedIdTimer,
                ._providerType = EventType::Timer,
                ._event = static_cast<nlohmann::json>(event).dump(),
            });
            if (problem._type != StorageCacheProblemType::NoProblems)
                throw std::runtime_error("Failed to create event");
            return event._id;
        };

        const Uuid eventId1 = addEvent(true);
        const Uuid eventId2 = addEvent(false);

        const auto addScenario = [&scenarioCache](const std::set<Uuid>& activate, const std::set<Uuid>& deactivate) -> Uuid {
            Scenario scenario{
                ._activateEvent = activate,
                ._deactivateEvent = deactivate,
            };
            const StorageCacheProblem problem = scenarioCache->InsertOrReplace(SimpleTableInsertOrReplaceInput<Scenario>{
                ._id = scenario._id,
                ._data = scenario,
            });
            if (problem._type != StorageCacheProblemType::NoProblems)
                throw std::runtime_error("Failed to create scenario");
            return scenario._id;
        };

        const Uuid scenarioId1 = addScenario({ eventId2 }, { eventId1 });
        const Uuid scenarioId2 = addScenario({ eventId1, Uuid{} }, { eventId2, Uuid{} });

        const auto checkEventState = [&eventCache](const Uuid& eventId, bool expectedActivity) {
            eventCache->Cleanup();
            EventTableSelectOutput eventOutput;
            const StorageCacheProblem problem = eventCache->Select(
                EventTableSelectInput{
                    ._id = eventId,
                    ._type = EventType::Timer,
                },
                eventOutput);
            if (problem._type != StorageCacheProblemType::NoProblems)
                throw std::runtime_error("Event check failed");
            for (const auto& eventString : eventOutput._data) {
                const Event event = nlohmann::json::parse(eventString).get<Event>();
                REQUIRE(event._active == expectedActivity);
            }
        };

        checkEventState(eventId1, true);
        checkEventState(eventId2, false);

        REQUIRE(ActivateScenario(&storage, scenarioId1));

        checkEventState(eventId1, false);
        checkEventState(eventId2, true);

        REQUIRE(ActivateScenario(&storage, scenarioId2));

        checkEventState(eventId1, true);
        checkEventState(eventId2, false);

    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
