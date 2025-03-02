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
    Timer,
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
};

enum class UniversalDataType : std::uint8_t {
    Bool,
    Int,
    Double,
    String,
};
