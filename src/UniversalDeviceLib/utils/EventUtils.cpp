#include "EventUtils.hpp"

#include <string>
#include <variant>

#include "Enums.hpp"
#include "Event.hpp"
#include "Uuid.hpp"

Uuid GetEventId(const Event& event) {
    return std::visit([](const auto& e) -> Uuid { return e._id; }, event);
}

std::string GetEventName(const Event& event) {
    return std::visit([](const auto& e) -> std::string { return e._name; }, event);
}

bool GetEventActivity(const Event& event) {
    return std::visit([](const auto& e) -> bool { return e._active; }, event);
}

EventType GetEventType(const Event& event) {
    return std::visit([](const auto& e) -> EventType { return e._type; }, event);
}
