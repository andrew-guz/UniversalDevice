#pragma once

#include <limits>

#include "BaseEvent.hpp"
#include "Enums.hpp"

struct ThermostatEvent final : public BaseEvent {
    const EventType _type = EventType::Thermostat;
    float _temperature = std::numeric_limits<float>::min();
    float _delta = 0.5f;
};
