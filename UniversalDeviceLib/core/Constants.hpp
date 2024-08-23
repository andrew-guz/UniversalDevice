#pragma once

#include "Uuid.hpp"

class Constants final {
public:
    // login
    static constexpr std::string_view LoginService = "service";
    static constexpr std::string_view LoginDevice = "device";

    // subjects - depends on type
    static constexpr std::string_view SubjectUndefined = "undefined_subject";
    static constexpr std::string_view SubjectTimerEvent = "timer_timeout";
    static constexpr std::string_view SubjectGetDeviceInformation = "get_device_information";
    static constexpr std::string_view SubjectThermometerCurrentValue = "thermometer_current_value";
    static constexpr std::string_view SubjectRelayCurrentState = "relay_current_state";
    static constexpr std::string_view SubjectMotionRelayCurrentState = "motion_relay_current_state";
    static constexpr std::string_view SubjectWebSocketAuthorization = "websocket_authorization";
    static constexpr std::string_view SubjectWebSocketGetSettings = "websocket_get_settings";
    static constexpr std::string_view SubjectWebSocketGetCommands = "websocket_get_commands";

    // predefined ids
    static const Uuid PredefinedIdTimer;
    static const Uuid PredefinedIdClient;
};
