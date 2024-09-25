#include <variant>

#include <catch2/catch_all.hpp>

#include "Enums.hpp"
#include "Marshaling.hpp"
#include "Types.hpp"

TEST_CASE("ActorTypeToString") {
    ActorType type;

    type = ClientActor{};
    REQUIRE(ActorTypeToString(type) == "client");

    type = DeviceType::Relay;
    REQUIRE(ActorTypeToString(type) == "relay");

    type = EventType::Thermostat;
    REQUIRE(ActorTypeToString(type) == "thermostat_event");
}

TEST_CASE("ActorTypeFromString") {
    ActorType type;

    type = ActorTypeFromString("client");
    REQUIRE(std::holds_alternative<ClientActor>(type));

    type = ActorTypeFromString("relay");
    REQUIRE(std::holds_alternative<DeviceType>(type));
    REQUIRE(std::get<DeviceType>(type) == DeviceType::Relay);

    type = ActorTypeFromString("thermostat_event");
    REQUIRE(std::holds_alternative<EventType>(type));
    REQUIRE(std::get<EventType>(type) == EventType::Thermostat);
}
