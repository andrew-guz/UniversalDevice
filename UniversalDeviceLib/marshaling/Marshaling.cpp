#include "Marshaling.hpp"

#include "Account.hpp"
#include "Base64Helper.hpp"
#include "ComponentDescription.hpp"
#include "CurrentTime.hpp"
#include "DeviceInformationDescription.hpp"
#include "DeviceProperty.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ExtendedComponentDescription.hpp"
#include "ExtendedMotionRelayCurrentState.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "ExtendedThermometerCurrentValue.hpp"
#include "LogInformation.hpp"
#include "Logger.hpp"
#include "Message.hpp"
#include "MessageHeader.hpp"
#include "MotionRelayCurrentState.hpp"
#include "MotionRelaySettings.hpp"
#include "PeriodSettings.hpp"
#include "RelayCurrentState.hpp"
#include "RelayEvent.hpp"
#include "RelayState.hpp"
#include "ThermometerCurrentValue.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerLedBrightness.hpp"
#include "ThermostatEvent.hpp"
#include "TimerEvent.hpp"
#include "Uuid.hpp"
#include "WebSocketAuthentication.hpp"
#include <string>

std::string EventTypeToString(EventType eventType) {
    switch (eventType) {
        case EventType::Undefined:
            return "undefined _event";
        case EventType::Timer:
            return "timer_event";
        case EventType::Thermometer:
            return "thermometer_event";
        case EventType::Relay:
            return "relay_event";
        case EventType::Thermostat:
            return "thermostat_event";
    }
    LOG_ERROR << "Invalid EventType: " << static_cast<int>(eventType) << std::endl;
    return {};
}

EventType EventTypeFromString(const std::string& str) {
    if (str == "timer_event")
        return EventType::Timer;
    if (str == "thermometer_event")
        return EventType::Thermometer;
    if (str == "relay_event")
        return EventType::Relay;
    if (str == "thermostat_event")
        return EventType::Thermostat;
    LOG_ERROR << "Invalid EventType: " << str << std::endl;
    return EventType::Undefined;
}

void to_json(nlohmann::json& json, const Uuid& uuid) { json = uuid.data(); }

void from_json(const nlohmann::json& json, Uuid& uuid) { uuid = Uuid(json.get<std::string>()); }

void to_json(nlohmann::json& json, const EventType eventType) { json = EventTypeToString(eventType); }

void from_json(const nlohmann::json& json, EventType& eventType) { eventType = EventTypeFromString(json.get<std::string>()); }

void to_json(nlohmann::json& json, const Account& account) {
    json = {
        { "login", account._login },
        { "password", account._password },
    };
}

void from_json(const nlohmann::json& json, Account& account) {
    account._login = json.value("login", std::string());
    account._password = json.value("password", std::string());
}

void to_json(nlohmann::json& json, const ComponentDescription& componentDescription) {
    json = {
        { "type", componentDescription._type },
        { "id", componentDescription._id.data() },
    };
}

void from_json(const nlohmann::json& json, ComponentDescription& componentDescription) {
    componentDescription._type = json["type"].get<std::string>();
    componentDescription._id = Uuid(json.value("id", ""));
}

void to_json(nlohmann::json& json, const CurrentTime& currentTime) {
    json = {
        { "timestamp", TimeHelper::TimeToInt(currentTime._timestamp) },
    };
}

void from_json(const nlohmann::json& json, CurrentTime& currentTime) {
    currentTime._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const DeviceInformationDescription& deviceInformationDescription) {
    to_json(json, (const ComponentDescription&)deviceInformationDescription);
    json += { "seconds", deviceInformationDescription._seconds };
}

void from_json(const nlohmann::json& json, DeviceInformationDescription& deviceInformationDescription) {
    from_json(json, (ComponentDescription&)deviceInformationDescription);
    deviceInformationDescription._seconds = json.value("seconds", (uint64_t)0);
}

void to_json(nlohmann::json& json, const DeviceProperty& deviceProperty) {
    json = {
        { "value", deviceProperty._value },
    };
}

void from_json(const nlohmann::json& json, DeviceProperty& deviceProperty) { deviceProperty._value = json.value("value", std::string()); }

void to_json(nlohmann::json& json, const Event& event) {
    json = {
        { "id", event._id.data() },      { "name", event._name },         { "active", event._active },   { "type", event._type },
        { "provider", event._provider }, { "receiver", event._receiver }, { "command", event._command },
    };
}

void from_json(const nlohmann::json& json, Event& event) {
    event._id = Uuid(json.value("id", ""));
    event._name = json.value("name", "");
    event._active = json.value("active", true);
    event._type = json["type"].get<EventType>();
    event._provider = json["provider"].get<ComponentDescription>();
    event._receiver = json["receiver"].get<ComponentDescription>();
    event._command = json["command"];
}

void to_json(nlohmann::json& json, const ExtendedComponentDescription& extendedComponentDescription) {
    to_json(json, (const ComponentDescription&)extendedComponentDescription);
    json += { "name", extendedComponentDescription._name };
    json += { "grp", extendedComponentDescription._group };
    json += { "timestamp", TimeHelper::TimeToInt(extendedComponentDescription._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedComponentDescription& extendedComponentDescription) {
    from_json(json, (ComponentDescription&)extendedComponentDescription);
    extendedComponentDescription._name = json.value("name", "");
    extendedComponentDescription._group = json.value("grp", "");
    extendedComponentDescription._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedMotionRelayCurrentState& extendedMotionRelayCurrentState) {
    to_json(json, (const MotionRelayCurrentState&)extendedMotionRelayCurrentState);
    json += { "timestamp", TimeHelper::TimeToInt(extendedMotionRelayCurrentState._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedMotionRelayCurrentState& extendedMotionRelayCurrentState) {
    from_json(json, (MotionRelayCurrentState&)extendedMotionRelayCurrentState);
    extendedMotionRelayCurrentState._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedRelayCurrentState& extendedRelayCurrentState) {
    to_json(json, (const RelayCurrentState&)extendedRelayCurrentState);
    json += { "timestamp", TimeHelper::TimeToInt(extendedRelayCurrentState._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedRelayCurrentState& extendedRelayCurrentState) {
    from_json(json, (RelayCurrentState&)extendedRelayCurrentState);
    extendedRelayCurrentState._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedThermometerCurrentValue& extendedThermometerCurrentValue) {
    to_json(json, (const ThermometerCurrentValue&)extendedThermometerCurrentValue);
    json += { "timestamp", TimeHelper::TimeToInt(extendedThermometerCurrentValue._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedThermometerCurrentValue& extendedThermometerCurrentValue) {
    from_json(json, (ThermometerCurrentValue&)extendedThermometerCurrentValue);
    extendedThermometerCurrentValue._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const LogInformation& logInformation) {
    json = {
        { "fileName", logInformation._fileName },
        { "fileContent", Base64Helper::ToBase64(logInformation._fileContent) },
    };
}

void from_json(const nlohmann::json& json, LogInformation& logInformation) {
    logInformation._fileName = json.value("fileName", "");
    logInformation._fileContent = Base64Helper::FromBase64(json.value("fileContent", ""));
}

void to_json(nlohmann::json& json, const Message& message) {
    json = {
        { "header", message._header },
        { "data", message._data },
    };
}

void from_json(const nlohmann::json& json, Message& message) {
    message._header = json["header"].get<MessageHeader>();
    message._data = json["data"];
}

void to_json(nlohmann::json& json, const MessageHeader& messageHeader) {
    json = {
        { "id", messageHeader._id },
        { "description", messageHeader._description },
        { "subject", messageHeader._subject },
    };
}

void from_json(const nlohmann::json& json, MessageHeader& messageHeader) {
    // since old devices have no id in header
    messageHeader._id = json.contains("id") ? json["id"].get<Uuid>() : Uuid();
    messageHeader._description = json["description"].get<ComponentDescription>();
    messageHeader._subject = json.value("subject", Constants::SubjectUndefined);
}

void to_json(nlohmann::json& json, const MotionRelayCurrentState& motionRelayCurrentState) {
    json = {
        { "motion", motionRelayCurrentState._motion },
        { "state", motionRelayCurrentState._state },
    };
}

void from_json(const nlohmann::json& json, MotionRelayCurrentState& motionRelayCurrentState) {
    motionRelayCurrentState._motion = json.value("motion", false);
    motionRelayCurrentState._state = json.value("state", std::numeric_limits<float>::min());
}

void to_json(nlohmann::json& json, const MotionRelaySettings& motionRelaySettings) {
    to_json(json, (const PeriodSettings&)motionRelaySettings);
    json += { "activityTime", motionRelaySettings._activityTime };
}

void from_json(const nlohmann::json& json, MotionRelaySettings& motionRelaySettings) {
    from_json(json, (PeriodSettings&)motionRelaySettings);
    motionRelaySettings._activityTime = json.value("activityTime", DEFAULT_ACTIVITY_TIME);
}

void to_json(nlohmann::json& json, const PeriodSettings& periodSettings) {
    json = {
        { "period", periodSettings._period },
    };
}

void from_json(const nlohmann::json& json, PeriodSettings& periodSettings) { periodSettings._period = json.value("period", DEFAULT_PERIOD); }

void to_json(nlohmann::json& json, const RelayCurrentState& relayCurrentState) {
    json = {
        { "state", relayCurrentState._state },
    };
}

void from_json(const nlohmann::json& json, RelayCurrentState& relayCurrentState) {
    relayCurrentState._state = json.value("state", std::numeric_limits<float>::min());
}

void to_json(nlohmann::json& json, const RelayEvent& relayEvent) {
    to_json(json, (const Event&)relayEvent);
    json += { "state", relayEvent._state };
}

void from_json(const nlohmann::json& json, RelayEvent& relayEvent) {
    from_json(json, (Event&)relayEvent);
    relayEvent._state = json.value("state", 0);
}

void to_json(nlohmann::json& json, const RelayState& relayState) {
    json = {
        { "state", relayState._state },
    };
}

void from_json(const nlohmann::json& json, RelayState& relayState) { relayState._state = json.value("state", std::numeric_limits<float>::min()); }

void to_json(nlohmann::json& json, const ThermometerLedBrightness& thermometerLedBrightness) {
    json = {
        { "brightness", thermometerLedBrightness._brightness },
    };
}

void from_json(const nlohmann::json& json, ThermometerLedBrightness& thermometerLedBrightness) {
    thermometerLedBrightness._brightness = json.value("brightness", 7);
}

void to_json(nlohmann::json& json, const ThermometerCurrentValue& thermometerCurrentValue) {
    json = {
        { "value", thermometerCurrentValue._value },
    };
}

void from_json(const nlohmann::json& json, ThermometerCurrentValue& thermometerCurrentValue) {
    thermometerCurrentValue._value = json.value("value", std::numeric_limits<float>::min());
}

void to_json(nlohmann::json& json, const ThermometerEvent& thermometerEvent) {
    to_json(json, (const Event&)thermometerEvent);
    json += { "temperature", thermometerEvent._temperature };
    json += { "lower", thermometerEvent._lower };
}

void from_json(const nlohmann::json& json, ThermometerEvent& thermometerEvent) {
    from_json(json, (Event&)thermometerEvent);
    thermometerEvent._temperature = json.value("temperature", std::numeric_limits<float>::min());
    thermometerEvent._lower = json.value("lower", true);
}

void to_json(nlohmann::json& json, const ThermostatEvent& thermostatEvent) {
    to_json(json, (const Event&)thermostatEvent);
    json += { "temperature", thermostatEvent._temperature };
    json += { "delta", thermostatEvent._delta };
}

void from_json(const nlohmann::json& json, ThermostatEvent& thermostatEvent) {
    from_json(json, (Event&)thermostatEvent);
    thermostatEvent._temperature = json.value("temperature", std::numeric_limits<float>::min());
    thermostatEvent._delta = json.value("delta", 0.5f);
}

void to_json(nlohmann::json& json, const TimerEvent& timerEvent) {
    to_json(json, (const Event&)timerEvent);
    json += { "hour", timerEvent._hour };
    json += { "minute", timerEvent._minute };
}

void from_json(const nlohmann::json& json, TimerEvent& timerEvent) {
    from_json(json, (Event&)timerEvent);
    timerEvent._hour = json.value("hour", 0);
    timerEvent._minute = json.value("minute", 0);
}

void to_json(nlohmann::json& json, const WebSocketAuthentication& webSocketAuthentication) {
    json = {
        { "authString", webSocketAuthentication._authString },
    };
}

void from_json(const nlohmann::json& json, WebSocketAuthentication& webSocketAuthentication) {
    webSocketAuthentication._authString = json.value("authString", "");
}
