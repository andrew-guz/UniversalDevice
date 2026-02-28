#pragma once

#include <limits>

#include "BaseEvent.hpp"
#include "Command.hpp"
#include "Enums.hpp"

struct ThermometerEvent final : public BaseEvent {
    const EventType _type = EventType::Thermometer;
    float _temperature = std::numeric_limits<float>::min();
    bool _lower = true;
    Command _command;
};
