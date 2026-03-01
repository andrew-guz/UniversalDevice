#include <filesystem>
#include <optional>
#include <variant>

#include <boost/hof/first_of.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "Event.hpp"
#include "EventUtils.hpp"
#include "EventsController.hpp"
#include "Provider.hpp"
#include "Storage.hpp"
#include "TimerEvent.hpp"
#include "Uuid.hpp"

TEST_CASE("EventsController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        EventsController eventController{ &storage };

        REQUIRE(eventController.List().size() == 0);
        REQUIRE(eventController.GetByProvider(Uuid{}).size() == 0);
        REQUIRE(eventController.GetById(Uuid{}).has_value() == false);

        TimerEvent timerEvent{
            ._hour = 1,
            ._minute = 2,
        };
        timerEvent._provider = ClientProvider{};
        Event event = timerEvent;
        REQUIRE(eventController.Add(event) == true);

        REQUIRE(eventController.List().size() == 1);
        REQUIRE(eventController.GetById(GetEventId(event)).has_value() == true);

        std::visit(boost::hof::first_of([](TimerEvent& timerEvent) { timerEvent._hour = 3; }, [](const auto&) {}), event);

        std::optional<Event> updatedEvent = eventController.Update(event);
        REQUIRE(updatedEvent.has_value() == true);
        REQUIRE(std::visit(boost::hof::first_of([](const TimerEvent& timerEvent) -> bool { return timerEvent._hour == 3; },
                                                [](const auto&) -> bool { return false; }),
                           event) == true);

        std::optional<Event> updatedEvent2 = eventController.SetActivity(GetEventId(updatedEvent.value()), false);
        REQUIRE(updatedEvent2.has_value() == true);
        REQUIRE(GetEventActivity(updatedEvent2.value()) == false);

        std::optional<Event> updatedEvent3 = eventController.SetActivity(Uuid{}, false);
        REQUIRE(updatedEvent3.has_value() == false);

        std::optional<Event> updatedEvent4 = eventController.SetActivity(Uuid{}, true);
        REQUIRE(updatedEvent4.has_value() == false);

        REQUIRE(eventController.Delete(GetEventId(event)) == true);
        REQUIRE(eventController.Delete(Uuid{}) == false);

        REQUIRE(eventController.List().size() == 0);
        REQUIRE(eventController.GetById(GetEventId(event)).has_value() == false);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
