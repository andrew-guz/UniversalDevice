#pragma once

#include <cstdint>

/* ATTENTION - add new always to the end */

enum class AccountType : std::uint8_t {
    Undefined,
    Viewer,
    User,
    Admin,
    Internal,
};

enum class DeviceType : std::uint8_t {
    Undefined,
    Thermometer,
    Relay,
    MotionRelay,
    UniversalDevice,
};

enum class EventType : std::uint8_t {
    Undefined,
    Timer,
    Thermometer,
    Relay,
    Thermostat,
    Sunrise,
    Sunset,
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
    UniversalDeviceCurrentState,
    SunriseEvent,
    SunsetEvent,
};

enum class UniversalDataType : std::uint8_t {
    Bool,
    Int,
    Double,
    String,
};
