#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Uuid.h"

class Constants final
{
public:
    //login
    static constexpr std::string_view LoginService = "service";
    static constexpr std::string_view LoginDevice = "device";

    // device types   
    static constexpr std::string_view DeviceTypeUndefined = "undefined_type";
    static constexpr std::string_view DeviceTypeTimer = "timer";
    static constexpr std::string_view DeviceTypeThermometer = "thermometer";
    static constexpr std::string_view DeviceTypeRelay = "relay";
    static constexpr std::string_view DeviceTypeMotionRelay = "motion_relay";

    //subjects - depends on type
    static constexpr std::string_view SubjectUndefined = "undefined_subject";
    static constexpr std::string_view SubjectTimerEvent = "timer_timeout";
    static constexpr std::string_view SubjectGetDeviceInformation = "get_device_information";
    static constexpr std::string_view SubjectThermometerCurrentValue = "thermometer_current_value";
    static constexpr std::string_view SubjectRelayCurrentState = "relay_current_state";
    static constexpr std::string_view SubjectMotionRelayCurrentState = "motion_relay_current_state";
    static constexpr std::string_view SubjectWebSocketAuthorization = "websocket_authorization";
    static constexpr std::string_view SubjectWebSocketGetSettings = "websocket_get_settings";
    static constexpr std::string_view SubjectWebSocketGetCommands = "websocket_get_commands";

    //predefined ids for devices
    static const Uuid PredefinedIdTimer;

    //event types
    static constexpr std::string_view EventTypeUndefined = "undefined _event";
    static constexpr std::string_view EventTypeTimer = "timer_event";
    static constexpr std::string_view EventTypeThermometer = "thermometer_event";
    static constexpr std::string_view EventTypeRelay = "relay_event";
    static constexpr std::string_view EventTypeThermostat = "thermostat_event";
};

#endif //_CONSTANTS_H_
