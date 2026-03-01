#pragma once

#include "BaseEvent.hpp"
#include "Command.hpp"
#include "Enums.hpp"

struct RelayEvent final : public BaseEvent {
    const EventType _type = EventType::Relay;
    int _state = 0;
    Command _command;
};
