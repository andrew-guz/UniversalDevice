#pragma once

#include "Enums.hpp"
#include "Event.hpp"

struct RelayEvent final : public Event {
    int _state = 0;

    RelayEvent() :
        Event(EventType::Relay) {}

    virtual ~RelayEvent() = default;
};
