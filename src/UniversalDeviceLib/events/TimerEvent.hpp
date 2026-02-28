#pragma once

#include "BaseEvent.hpp"
#include "Command.hpp"
#include "Enums.hpp"

struct TimerEvent final : public BaseEvent {
    const EventType _type = EventType::Timer;
    int _hour = 0;
    int _minute = 0;
    Command _command;
};
