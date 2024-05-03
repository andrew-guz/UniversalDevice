#pragma once

#include "Event.hpp"

struct TimerEvent final : public Event {
    int _hour = 0;
    int _minute = 0;

    TimerEvent() : Event(Constants::EventTypeTimer) {}

    virtual ~TimerEvent() = default;
};
