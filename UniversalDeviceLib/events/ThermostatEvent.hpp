#pragma once

#include <limits>

#include "Enums.hpp"
#include "Event.hpp"

struct ThermostatEvent final : public Event {
    float _temperature = std::numeric_limits<float>::min();
    float _delta = 0.5f;

    ThermostatEvent() : Event(EventType::Thermostat) {}

    virtual ~ThermostatEvent() = default;
};
