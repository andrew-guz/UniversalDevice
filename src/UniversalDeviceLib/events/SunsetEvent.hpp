#pragma once

#include "BaseEvent.hpp"
#include "Command.hpp"
#include "Enums.hpp"

struct SunsetEvent final : public BaseEvent {
    const EventType _type = EventType::Sunset;
    Command _command;
};
