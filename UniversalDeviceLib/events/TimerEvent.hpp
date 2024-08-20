#pragma once

#include "Enums.hpp"
#include "Event.hpp"

struct TimerEvent final : public Event {
    int _hour = 0;
    int _minute = 0;

    TimerEvent() : Event(EventType::Timer) {}

    virtual ~TimerEvent() = default;
};
