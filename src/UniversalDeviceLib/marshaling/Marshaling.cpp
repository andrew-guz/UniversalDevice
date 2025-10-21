#include "Marshaling.hpp"

#include <cstdint>
#include <set>
#include <string>
#include <string_view>
#include <variant>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "Account.hpp"
#include "Base64Helper.hpp"
#include "ComponentDescription.hpp"
#include "CurrentTime.hpp"
#include "Device.hpp"
#include "DeviceInformationDescription.hpp"
#include "DeviceProperty.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "ExtendedComponentDescription.hpp"
#include "ExtendedMotionRelayCurrentState.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "ExtendedThermometerCurrentValue.hpp"
#include "ExtendedUniversalDeviceCurrentValues.hpp"
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
#include "RelayValue.hpp"
#include "Scenario.hpp"
#include "Settings.hpp"
#include "SunriseEvent.hpp"
#include "SunsetEvent.hpp"
#include "ThermometerCurrentValue.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerLedBrightness.hpp"
#include "ThermometerValue.hpp"
#include "ThermostatEvent.hpp"
#include "TimeHelper.hpp"
#include "TimerEvent.hpp"
#include "Types.hpp"
#include "UniversalData.hpp"
#include "UniversalDeviceCurrentValues.hpp"
#include "Uuid.hpp"
#include "WebSocketAuthentication.hpp"

template<>
std::string EnumToString(AccountType enumType) {
    switch (enumType) {
        case AccountType::Undefined:
            return "undefined";
        case AccountType::Viewer:
            return "viewer";
        case AccountType::User:
            return "user";
        case AccountType::Admin:
            return "admin";
        case AccountType::Internal:
            return "internal";
    }
    LOG_ERROR_MSG(fmt::format("Invalid AccountType: {}", static_cast<int>(enumType)));
    return {};
}

AccountType AccountTypeFromString(const std::string& str) {
    if (str == "undefined")
        return AccountType::Undefined;
    if (str == "viewer")
        return AccountType::Viewer;
    if (str == "user")
        return AccountType::User;
    if (str == "admin")
        return AccountType::Admin;
    if (str == "internal")
        return AccountType::Internal;
    return AccountType::Undefined;
}

template<>
AccountType EnumFromString(const std::string& str) {
    return AccountTypeFromString(str);
}

template<>
std::string EnumToString(DeviceType enumType) {
    switch (enumType) {
        case DeviceType::Undefined:
            return "undefined_type";
        case DeviceType::Thermometer:
            return "thermometer";
        case DeviceType::Relay:
            return "relay";
        case DeviceType::MotionRelay:
            return "motion_relay";
        case DeviceType::UniversalDevice:
            return "universal_device";
    }
    LOG_ERROR_MSG(fmt::format("Invalid DeviceType: {}", static_cast<int>(enumType)));
    return {};
}

DeviceType DeviceTypeFromString(const std::string& str) {
    if (str == "thermometer")
        return DeviceType::Thermometer;
    if (str == "relay")
        return DeviceType::Relay;
    if (str == "motion_relay")
        return DeviceType::MotionRelay;
    if (str == "universal_device")
        return DeviceType::UniversalDevice;
    return DeviceType::Undefined;
}

template<>
DeviceType EnumFromString(const std::string& str) {
    const DeviceType deviceType = DeviceTypeFromString(str);
    if (deviceType == DeviceType::Undefined)
        LOG_ERROR_MSG(fmt::format("Invalid DeviceType: {}", str));
    return deviceType;
}

template<>
std::string EnumToString(EventType enumType) {
    switch (enumType) {
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
        case EventType::Sunrise:
            return "sunrise_event";
        case EventType::Sunset:
            return "sunset_event";
    }
    LOG_ERROR_MSG(fmt::format("Invalid EventType: {}", static_cast<int>(enumType)));
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
    if (str == "sunrise_event")
        return EventType::Sunrise;
    if (str == "sunset_event")
        return EventType::Sunset;
    return EventType::Undefined;
}

template<>
EventType EnumFromString(const std::string& str) {
    const EventType eventType = EventTypeFromString(str);
    if (eventType == EventType::Undefined)
        LOG_ERROR_MSG(fmt::format("Invalid EventType: {}", str));
    return eventType;
}

template<>
LogLevel EnumFromString(const std::string& str) {
    if (str == "DEBUG")
        return LogLevel::DEBUG;
    else if (str == "INFO")
        return LogLevel::INFO;
    else if (str == "ERROR")
        return LogLevel::ERROR;
    LOG_ERROR_MSG(fmt::format("Invalid LogLevel: {}", str));
    return LogLevel::INFO;
}

template<>
std::string EnumToString(Subject enumType) {
    switch (enumType) {
        case Subject::Undefined:
            return "undefined_subject";
        case Subject::TimerEvent:
            return "timer_timeout";
        case Subject::GetDeviceInformation:
            return "get_device_information";
        case Subject::ThermometerCurrentValue:
            return "thermometer_current_value";
        case Subject::RelayCurrentState:
            return "relay_current_state";
        case Subject::MotionRelayCurrentState:
            return "motion_relay_current_state";
        case Subject::WebSocketAuthorization:
            return "websocket_authorization";
        case Subject::WebSocketGetSettings:
            return "websocket_get_settings";
        case Subject::WebSocketGetCommands:
            return "websocket_get_commands";
        case Subject::UniversalDeviceCurrentState:
            return "universal_device_current_state";
        case Subject::SunriseEvent:
            return "sunrise_timeout";
        case Subject::SunsetEvent:
            return "sunset_timeout";
    }
    LOG_ERROR_MSG(fmt::format("Invalid SubjectType: {}", static_cast<int>(enumType)));
    return {};
}

template<>
Subject EnumFromString(const std::string& str) {
    if (str == "timer_timeout")
        return Subject::TimerEvent;
    if (str == "get_device_information")
        return Subject::GetDeviceInformation;
    if (str == "thermometer_current_value")
        return Subject::ThermometerCurrentValue;
    if (str == "relay_current_state")
        return Subject::RelayCurrentState;
    if (str == "motion_relay_current_state")
        return Subject::MotionRelayCurrentState;
    if (str == "websocket_authorization")
        return Subject::WebSocketAuthorization;
    if (str == "websocket_get_settings")
        return Subject::WebSocketGetSettings;
    if (str == "websocket_get_commands")
        return Subject::WebSocketGetCommands;
    if (str == "universal_device_current_state")
        return Subject::UniversalDeviceCurrentState;
    if (str == "sunrise_timeout")
        return Subject::SunriseEvent;
    if (str == "sunset_timeout")
        return Subject::SunsetEvent;
    LOG_ERROR_MSG(fmt::format("Invalid SubjectType: {}", str));
    return Subject::Undefined;
}

template<>
std::string EnumToString(UniversalDataType enumType) {
    switch (enumType) {
        case UniversalDataType::Bool:
            return "boolean";
        case UniversalDataType::Int:
            return "integer";
        case UniversalDataType::Double:
            return "double";
        case UniversalDataType::String:
            return "string";
    }
    LOG_ERROR_MSG(fmt::format("Invalid UniversalDataType: {}", static_cast<int>(enumType)));
    return {};
}

template<>
UniversalDataType EnumFromString(const std::string& str) {
    if (str == "boolean")
        return UniversalDataType::Bool;
    if (str == "integer")
        return UniversalDataType::Int;
    if (str == "double")
        return UniversalDataType::Double;
    if (str == "string")
        return UniversalDataType::String;
    LOG_ERROR_MSG(fmt::format("Invalid UniversalDataType: {}", str));
    return UniversalDataType::Bool;
}

std::string ActorTypeToString(const ActorType& type) {
    switch (type.index()) {
        case 0: // ClientActor
            return "client";
        case 1: // DeviceType
            return EnumToString<DeviceType>(std::get<DeviceType>(type));
        case 2: // EventType
            return EnumToString<EventType>(std::get<EventType>(type));
    }
    LOG_ERROR_MSG(fmt::format("Invalid Actor Type: {}", type.index()));
    return {};
}

ActorType ActorTypeFromString(const std::string& str) {
    if (str == "client")
        return ClientActor{};
    const DeviceType deviceType = DeviceTypeFromString(str);
    if (deviceType != DeviceType::Undefined)
        return deviceType;
    const EventType eventType = EventTypeFromString(str);
    if (eventType != EventType::Undefined)
        return eventType;
    LOG_ERROR_MSG(fmt::format("Invalid Actor Type: {}", str));
    return ClientActor{};
}

void to_json(nlohmann::json& json, AccountType accountType) { json = EnumToString<AccountType>(accountType); }

void from_json(const nlohmann::json& json, AccountType& accountType) { accountType = EnumFromString<AccountType>(json.get<std::string>()); }

void to_json(nlohmann::json& json, DeviceType deviceType) { json = EnumToString<DeviceType>(deviceType); }

void from_json(const nlohmann::json& json, DeviceType& deviceType) { deviceType = EnumFromString<DeviceType>(json.get<std::string>()); }

void to_json(nlohmann::json& json, const EventType eventType) { json = EnumToString<EventType>(eventType); }

void from_json(const nlohmann::json& json, EventType& eventType) { eventType = EnumFromString<EventType>(json.get<std::string>()); }

void to_json(nlohmann::json& json, Subject subject) { json = EnumToString<Subject>(subject); }

void from_json(const nlohmann::json& json, Subject& subject) { subject = EnumFromString<Subject>(json.get<std::string>()); }

void to_json(nlohmann::json& json, const ActorType& type) { json = ActorTypeToString(type); }

void from_json(const nlohmann::json& json, ActorType& type) { type = ActorTypeFromString(json.get<std::string>()); }

void to_json(nlohmann::json& json, const Uuid& uuid) { json = uuid.data(); }

void from_json(const nlohmann::json& json, Uuid& uuid) { uuid = Uuid(json.get<std::string>()); }

void to_json(nlohmann::json& json, const Account& account) {
    json = {
        { "login", account._login },
        { "password", account._password },
        { "type", account._type },
    };
}

void from_json(const nlohmann::json& json, Account& account) {
    account._login = json.value("login", std::string());
    account._password = json.value("password", std::string());
    account._type = json.contains("type") ? json.at("type").get<AccountType>() : AccountType::Undefined;
}

void to_json(nlohmann::json& json, const ComponentDescription& componentDescription) {
    json = {
        { "type", componentDescription._type },
        { "id", componentDescription._id.data() },
    };
}

void from_json(const nlohmann::json& json, ComponentDescription& componentDescription) {
    componentDescription._type = json["type"].get<ActorType>();
    componentDescription._id = json["id"].get<Uuid>();
}

void to_json(nlohmann::json& json, const CurrentTime& currentTime) {
    json = {
        { "timestamp", TimeHelper::TimeToInt(currentTime._timestamp) },
    };
}

void from_json(const nlohmann::json& json, CurrentTime& currentTime) {
    currentTime._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const Device& device) {
    json = nlohmann::json{
        { "id", device._id.data() },
        { "type", device._type },
        { "name", device._name },
        { "grp", device._group },
        { "timestamp", TimeHelper::TimeToInt(device._timestamp) },
    };
}

void from_json(const nlohmann::json& json, Device& device) {
    device._id = json["id"].get<Uuid>();
    device._type = json["type"].get<DeviceType>();
    device._name = json["name"].get<std::string>();
    device._group = json["grp"].get<std::string>();
    device._timestamp = TimeHelper::TimeFromInt(json["timestamp"].get<int64_t>());
}

void to_json(nlohmann::json& json, const DeviceInformationDescription& deviceInformationDescription) {
    json = (const ComponentDescription&)deviceInformationDescription;
    json += { "seconds", deviceInformationDescription._seconds };
}

void from_json(const nlohmann::json& json, DeviceInformationDescription& deviceInformationDescription) {
    (ComponentDescription&)deviceInformationDescription = json.get<ComponentDescription>();
    deviceInformationDescription._seconds = json.value("seconds", (std::uint64_t)0);
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
    event._id = json["id"].get<Uuid>();
    event._name = json.value("name", "");
    event._active = json.value("active", true);
    event._type = json["type"].get<EventType>();
    event._provider = json["provider"].get<ComponentDescription>();
    event._receiver = json["receiver"].get<ComponentDescription>();
    event._command = json["command"];
}

void to_json(nlohmann::json& json, const ExtendedComponentDescription& extendedComponentDescription) {
    json = (const ComponentDescription&)extendedComponentDescription;
    json += { "name", extendedComponentDescription._name };
    json += { "grp", extendedComponentDescription._group };
    json += { "timestamp", TimeHelper::TimeToInt(extendedComponentDescription._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedComponentDescription& extendedComponentDescription) {
    (ComponentDescription&)extendedComponentDescription = json.get<ComponentDescription>();
    extendedComponentDescription._name = json.value("name", "");
    extendedComponentDescription._group = json.value("grp", "");
    extendedComponentDescription._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedMotionRelayCurrentState& extendedMotionRelayCurrentState) {
    json = (const MotionRelayCurrentState&)extendedMotionRelayCurrentState;
    json += { "timestamp", TimeHelper::TimeToInt(extendedMotionRelayCurrentState._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedMotionRelayCurrentState& extendedMotionRelayCurrentState) {
    (MotionRelayCurrentState&)extendedMotionRelayCurrentState = json.get<MotionRelayCurrentState>();
    extendedMotionRelayCurrentState._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedRelayCurrentState& extendedRelayCurrentState) {
    json = (const RelayCurrentState&)extendedRelayCurrentState;
    json += { "timestamp", TimeHelper::TimeToInt(extendedRelayCurrentState._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedRelayCurrentState& extendedRelayCurrentState) {
    (RelayCurrentState&)extendedRelayCurrentState = json.get<RelayCurrentState>();
    extendedRelayCurrentState._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedThermometerCurrentValue& extendedThermometerCurrentValue) {
    json = (const ThermometerCurrentValue&)extendedThermometerCurrentValue;
    json += { "timestamp", TimeHelper::TimeToInt(extendedThermometerCurrentValue._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedThermometerCurrentValue& extendedThermometerCurrentValue) {
    (ThermometerCurrentValue&)extendedThermometerCurrentValue = json.get<ThermometerCurrentValue>();
    extendedThermometerCurrentValue._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const ExtendedUniversalDeviceCurrentValues& extendedUniversalDeviceCurrentValues) {
    json = (const UniversalDeviceCurrentValues&)extendedUniversalDeviceCurrentValues;
    json += { "timestamp", TimeHelper::TimeToInt(extendedUniversalDeviceCurrentValues._timestamp) };
}

void from_json(const nlohmann::json& json, ExtendedUniversalDeviceCurrentValues& extendedUniversalDeviceCurrentValues) {
    (UniversalDeviceCurrentValues&)extendedUniversalDeviceCurrentValues = json.get<UniversalDeviceCurrentValues>();
    extendedUniversalDeviceCurrentValues._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
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

void from_json(const nlohmann::json& json, LogLevel& logLevel) { logLevel = EnumFromString<LogLevel>(json.get<std::string>()); }

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
    // TODO: do we really have message without Subject?
    messageHeader._subject = json.contains("subject") ? json["subject"].get<Subject>() : Subject::Undefined;
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
    json = (const PeriodSettings&)motionRelaySettings;
    json += { "activityTime", motionRelaySettings._activityTime };
}

void from_json(const nlohmann::json& json, MotionRelaySettings& motionRelaySettings) {
    (PeriodSettings&)motionRelaySettings = json.get<PeriodSettings>();
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

void to_json(nlohmann::json& json, const Settings& settings) {
    std::visit([&json](const auto& value) -> void { json = static_cast<nlohmann::json>(value); }, settings);
}

void from_json(const nlohmann::json& json, Settings& settings) {
    if (json.find("activityTime") != json.end())
        settings = json.get<MotionRelaySettings>();
    else if (json.find("period") != json.end())
        settings = json.get<PeriodSettings>();
    else
        LOG_ERROR_MSG(fmt::format("Invalid Settings: {}", json.dump()));
}

void from_json(const nlohmann::json& json, RelayCurrentState& relayCurrentState) {
    relayCurrentState._state = json.value("state", std::numeric_limits<float>::min());
}

void to_json(nlohmann::json& json, const RelayEvent& relayEvent) {
    json = (const Event&)relayEvent;
    json += { "state", relayEvent._state };
}

void from_json(const nlohmann::json& json, RelayEvent& relayEvent) {
    (Event&)relayEvent = json.get<Event>();
    relayEvent._state = json.value("state", 0);
}

void to_json(nlohmann::json& json, const RelayState& relayState) {
    json = {
        { "state", relayState._state },
    };
}

void from_json(const nlohmann::json& json, RelayState& relayState) { relayState._state = json.value("state", std::numeric_limits<float>::min()); }

void to_json(nlohmann::json& json, const RelayValue& relayValue) {
    json = {
        { "state", relayValue._state },
    };

    if (relayValue._timestamp.has_value())
        json += { "timestamp", TimeHelper::TimeToInt(relayValue._timestamp.value()) };
}

void from_json(const nlohmann::json& json, RelayValue& relayValue) {
    relayValue._state = json.value("value", std::numeric_limits<int>::min());

    if (json.contains("timestamp"))
        relayValue._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const Scenario& scenario) {
    json = {
        { "id", scenario._id },
        { "name", scenario._name },
        { "activate", scenario._activateEvent },
        { "deactivate", scenario._deactivateEvent },
    };
}

void from_json(const nlohmann::json& json, Scenario& scenario) {
    scenario._id = json.at("id").get<Uuid>();
    scenario._name = json.at("name").get<std::string>();
    scenario._activateEvent = json.at("activate").get<std::set<Uuid>>();
    scenario._deactivateEvent = json.at("deactivate").get<std::set<Uuid>>();
}

void to_json(nlohmann::json& json, const ThermometerLedBrightness& thermometerLedBrightness) {
    json = {
        { "brightness", thermometerLedBrightness._brightness },
    };
}

void from_json(const nlohmann::json& json, ThermometerLedBrightness& thermometerLedBrightness) {
    thermometerLedBrightness._brightness = json.value("brightness", 7);
}

void to_json(nlohmann::json& json, const Command& command) {
    std::visit([&json](const auto& value) -> void { json = static_cast<nlohmann::json>(value); }, command);
}

void from_json(const nlohmann::json& json, Command& command) {
    if (json.find("state") != json.end())
        command = json.get<RelayState>();
    else if (json.find("brightness") != json.end())
        command = json.get<ThermometerLedBrightness>();
    else
        LOG_ERROR_MSG(fmt::format("Invalid Command: {}", json.dump()));
}

void to_json(nlohmann::json& json, const ThermometerValue& thermometerValue) {
    json = {
        { "value", thermometerValue._value },
    };

    if (thermometerValue._timestamp.has_value())
        json += { "timestamp", TimeHelper::TimeToInt(thermometerValue._timestamp.value()) };
}

void from_json(const nlohmann::json& json, ThermometerValue& thermometerValue) {
    thermometerValue._value = json.value("value", std::numeric_limits<float>::min());

    if (json.contains("timestamp"))
        thermometerValue._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
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
    json = (const Event&)thermometerEvent;
    json += { "temperature", thermometerEvent._temperature };
    json += { "lower", thermometerEvent._lower };
}

void from_json(const nlohmann::json& json, ThermometerEvent& thermometerEvent) {
    (Event&)thermometerEvent = json.get<Event>();
    thermometerEvent._temperature = json.value("temperature", std::numeric_limits<float>::min());
    thermometerEvent._lower = json.value("lower", true);
}

void to_json(nlohmann::json& json, const ThermostatEvent& thermostatEvent) {
    json = (const Event&)thermostatEvent;
    json += { "temperature", thermostatEvent._temperature };
    json += { "delta", thermostatEvent._delta };
}

void from_json(const nlohmann::json& json, ThermostatEvent& thermostatEvent) {
    (Event&)thermostatEvent = json.get<Event>();
    thermostatEvent._temperature = json.value("temperature", std::numeric_limits<float>::min());
    thermostatEvent._delta = json.value("delta", 0.5f);
}

void to_json(nlohmann::json& json, const TimerEvent& timerEvent) {
    json = (const Event&)timerEvent;
    json += { "hour", timerEvent._hour };
    json += { "minute", timerEvent._minute };
}

void from_json(const nlohmann::json& json, TimerEvent& timerEvent) {
    (Event&)timerEvent = json.get<Event>();
    timerEvent._hour = json.value("hour", 0);
    timerEvent._minute = json.value("minute", 0);
}

void to_json(nlohmann::json& json, const SunriseEvent& sunriseEvent) { json = (const Event&)sunriseEvent; }

void from_json(const nlohmann::json& json, SunriseEvent& sunriseEvent) { (Event&)sunriseEvent = json.get<Event>(); }

void to_json(nlohmann::json& json, const SunsetEvent& sunsetEvent) { json = (const Event&)sunsetEvent; }

void from_json(const nlohmann::json& json, SunsetEvent& sunsetEvent) { (Event&)sunsetEvent = json.get<Event>(); }

namespace nlohmann {
    template<>
    struct adl_serializer<UniversalData> {
        static void to_json(json& json, const UniversalData& universalData) {
            json = nlohmann::json{
                { "type", EnumToString(universalData.GetType()) },
            };
            switch (universalData.GetType()) {
                case UniversalDataType::Bool:
                    json += { "value", universalData.Get<bool>() };
                    break;
                case UniversalDataType::Int:
                    json += { "value", universalData.Get<int>() };
                    break;
                case UniversalDataType::Double:
                    json += { "value", universalData.Get<double>() };
                    break;
                case UniversalDataType::String:
                    json += { "value", universalData.Get<std::string>() };
                    break;
            }
        }

        static UniversalData from_json(const json& json) {
            if (json.contains("type")) {
                const UniversalDataType type = EnumFromString<UniversalDataType>(json.at("type").get<std::string>());
                switch (type) {
                    case UniversalDataType::Bool:
                        return UniversalData(json.at("value").get<bool>());
                    case UniversalDataType::Int:
                        return UniversalData(json.at("value").get<int>());
                    case UniversalDataType::Double:
                        return UniversalData(json.at("value").get<double>());
                    case UniversalDataType::String:
                        return UniversalData(json.at("value").get<std::string>());
                }
            } else {
                LOG_ERROR_MSG("Invalid UniversalData type");
            }
            return UniversalData{ false };
        };
    };
} // namespace nlohmann

void to_json(nlohmann::json& json, const UniversalDeviceCurrentValues& universalDeviceCurrentValues) {
    json = nlohmann::json{
        { "values", universalDeviceCurrentValues._values },
    };
}

void from_json(const nlohmann::json& json, UniversalDeviceCurrentValues& universalDeviceCurrentValues) {
    const nlohmann::json& values = json.at("values");
    for (auto iter = values.begin(); iter != values.end(); ++iter) {
        universalDeviceCurrentValues._values.try_emplace(iter.key(), iter.value().get<UniversalData>());
    }
}

void to_json(nlohmann::json& json, const WebSocketAuthentication& webSocketAuthentication) {
    json = {
        { "authString", webSocketAuthentication._authString },
    };
}

void from_json(const nlohmann::json& json, WebSocketAuthentication& webSocketAuthentication) {
    webSocketAuthentication._authString = json.value("authString", "");
}
