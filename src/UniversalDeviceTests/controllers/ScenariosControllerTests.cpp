#include <filesystem>
#include <optional>
#include <set>
#include <stdexcept>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "Event.hpp"
#include "EventUtils.hpp"
#include "EventsController.hpp"
#include "Provider.hpp"
#include "Scenario.hpp"
#include "ScenariosController.hpp"
#include "Storage.hpp"
#include "TimerEvent.hpp"
#include "Uuid.hpp"

TEST_CASE("ScenariosControllerTests") {

    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        EventsController eventController{ &storage };
        ScenariosController scenariosController{ &storage, eventController };

        REQUIRE(scenariosController.List().size() == 0);
        REQUIRE(scenariosController.Get(Uuid{}).has_value() == false);

        TimerEvent timerEvent{
            ._hour = 1,
            ._minute = 2,
        };
        timerEvent._provider = ClientProvider{};
        Event event = timerEvent;
        REQUIRE(eventController.Add(event) == true);

        Scenario scenario{
            ._id = Uuid{},
            ._name = "test",
            ._activateEvent = { GetEventId(event) },
            ._deactivateEvent = {},
        };
        REQUIRE(scenariosController.Add(scenario) == true);

        REQUIRE(scenariosController.List().size() == 1);
        REQUIRE(scenariosController.Get(scenario._id).has_value() == true);
        REQUIRE(scenariosController.Get(scenario._id).value()._name == "test");

        scenario._name = "test2";
        REQUIRE(scenariosController.Update(scenario) == true);
        REQUIRE(scenariosController.Get(scenario._id).has_value() == true);
        REQUIRE(scenariosController.Get(scenario._id).value()._name == "test2");

        Scenario scenario2{
            ._id = Uuid{},
            ._name = "new",
            ._activateEvent = { Uuid{}, Uuid{} },
            ._deactivateEvent = { Uuid{}, Uuid{} },
        };
        REQUIRE(scenariosController.Add(scenario2) == true);
        REQUIRE(scenariosController.List().size() == 2);
        REQUIRE(scenariosController.Get(scenario2._id).has_value() == true);
        REQUIRE(scenariosController.Get(scenario2._id).value()._activateEvent.size() == 2);
        REQUIRE(scenariosController.Get(scenario2._id).value()._deactivateEvent.size() == 2);

        scenariosController.CleanupScenario(scenario2);
        REQUIRE(scenario2._activateEvent.size() == 0);
        REQUIRE(scenario2._deactivateEvent.size() == 0);

        REQUIRE(scenariosController.Remove(scenario2._id) == true);
        REQUIRE(scenariosController.List().size() == 1);
        REQUIRE(scenariosController.Remove(scenario._id) == true);
        REQUIRE(scenariosController.List().size() == 0);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("Scenario activation test") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {

        Storage storage{ dbPath };

        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController };

        const auto addEvent = [&eventsController](bool active) -> Uuid {
            TimerEvent event;
            event._provider = ClientProvider{};
            event._active = active;
            if (!eventsController.Add(event))
                throw std::runtime_error("Failed to create event");

            return event._id;
        };

        const Uuid eventId1 = addEvent(true);
        const Uuid eventId2 = addEvent(false);

        const auto addScenario = [&scenariosController](const std::set<Uuid>& activate, const std::set<Uuid>& deactivate) -> Uuid {
            Scenario scenario{
                ._activateEvent = activate,
                ._deactivateEvent = deactivate,
            };
            if (!scenariosController.Add(scenario))
                throw std::runtime_error("Failed to create scenario");

            return scenario._id;
        };

        const Uuid scenarioId1 = addScenario({ eventId2 }, { eventId1 });
        const Uuid scenarioId2 = addScenario({ eventId1, Uuid{} }, { eventId2, Uuid{} });

        const auto checkEventState = [&storage](const Uuid& eventId, bool expectedActivity) {
            EventsController eventsController{ &storage };
            const std::optional<Event> event = eventsController.GetById(eventId);
            if (!event.has_value())
                throw std::runtime_error("Event check failed");

            REQUIRE(GetEventActivity(event.value()) == expectedActivity);
        };

        checkEventState(eventId1, true);
        checkEventState(eventId2, false);

        REQUIRE(scenariosController.ActivateScenario(scenarioId1));

        checkEventState(eventId1, false);
        checkEventState(eventId2, true);

        REQUIRE(scenariosController.ActivateScenario(scenarioId2));

        checkEventState(eventId1, true);
        checkEventState(eventId2, false);

    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
