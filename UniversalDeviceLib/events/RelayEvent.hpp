#pragma once

#include "Event.hpp"

struct RelayEvent final : public Event {
    int _state = 0;

    RelayEvent() : Event(Constants::EventTypeRelay) {}

    virtual ~RelayEvent() = default;
};
