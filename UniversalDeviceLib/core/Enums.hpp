#pragma once

#include <cstdint>

enum class AccountType : std::uint8_t {
    Viewer,
    User,
    Admin,
    Internal,
};

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

enum class Subject : std::uint8_t {
    Undefined,
    TimerEvent,
    GetDeviceInformation,
    ThermometerCurrentValue,
    RelayCurrentState,
    MotionRelayCurrentState,
    WebSocketAuthorization,
    WebSocketGetSettings,
    WebSocketGetCommands,
};
