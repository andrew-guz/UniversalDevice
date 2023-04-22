#include "Constants.h"

//login
const std::string Constants::LoginService = "service";

const std::string Constants::LoginDevice = "device";

// device types
const std::string Constants::DeviceTypeUndefined = "undefined_type";

const std::string Constants::DeviceTypeTimer = "timer";

const std::string Constants::DeviceTypeThermometer = "thermometer";

const std::string Constants::DeviceTypeRelay = "relay";

const std::string Constants::DeviceTypeMotionRelay = "motion_relay";

//subjects - depends on type
const std::string Constants::SubjectUndefined = "undefined_subject";

const std::string Constants::SubjectTimerEvent = "timer_timeout";

const std::string Constants::SubjectGetDeviceInformation = "get_device_information";

const std::string Constants::SubjectThermometerCurrentValue = "thermometer_current_value";

const std::string Constants::SubjectRelayCurrentState = "relay_current_state";

const std::string Constants::SubjectMotionRelayCurrentState = "motion_relay_current_state";

const std::string Constants::SubjectWebSocketAuthorization = "websocket_authorization";

//predefined ids
const Uuid Constants::PredefinedIdTimer = Uuid("00000000-0000-0000-0000-000000000001");

//event types
const std::string Constants::EventTypeUndefined = "undefined _event";

const std::string Constants::EventTypeTimer = "timer_event";

const std::string Constants::EventTypeThermometer = "thermometer_event";

const std::string Constants::EventTypeRelay = "relay_event";

const std::string Constants::EventTypeThermostat = "thermostat_event";
