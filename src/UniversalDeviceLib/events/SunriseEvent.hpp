#pragma once

#include "Enums.hpp"
#include "Event.hpp"

struct SunriseEvent final : public Event {
    SunriseEvent() :
        Event(EventType::Sunrise) {}

    virtual ~SunriseEvent() = default;
};
