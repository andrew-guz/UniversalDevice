#pragma once

#include "BaseEvent.hpp"
#include "Command.hpp"
#include "Enums.hpp"

struct SunriseEvent final : public BaseEvent {
    const EventType _type = EventType::Sunrise;
    Command _command;
};
