#pragma once

#include "Enums.hpp"
#include "Event.hpp"

struct SunsetEvent final : public Event {
    SunsetEvent() :
        Event(EventType::Sunset) {}

    virtual ~SunsetEvent() = default;
};
