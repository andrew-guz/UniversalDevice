#pragma once

#include <cstdint>

enum class DeviceType : std::uint8_t {
    Undefined,
    Timer,
    Thermometer,
    Relay,
    MotionRelay,
};

enum class EventType : std::uint8_t {
    Undefined,
    Timer,
    Thermometer,
    Relay,
    Thermostat,
};
