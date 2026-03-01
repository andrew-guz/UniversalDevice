#include "Marshaling.hpp"

#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <variant>

#include <boost/hof.hpp>
#include <boost/hof/match.hpp>
#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "Account.hpp"
#include "Base64Helper.hpp"
#include "BaseEvent.hpp"
#include "Command.hpp"
#include "Constants.hpp"
#include "Defines.hpp"
#include "Device.hpp"
#include "DeviceDescription.hpp"
#include "DeviceInformationDescription.hpp"
#include "DeviceProperty.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "LogInformation.hpp"
#include "Logger.hpp"
#include "Message.hpp"
#include "MessageHeader.hpp"
#include "MotionRelaySettings.hpp"
#include "MotionRelayValue.hpp"
#include "PeriodSettings.hpp"
#include "Provider.hpp"
#include "Receiver.hpp"
#include "RelayEvent.hpp"
#include "RelayState.hpp"
#include "RelayValue.hpp"
#include "Scenario.hpp"
#include "Settings.hpp"
#include "SunriseEvent.hpp"
#include "SunsetEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerLedBrightness.hpp"
#include "ThermometerValue.hpp"
#include "ThermostatEvent.hpp"
#include "TimeHelper.hpp"
#include "TimerEvent.hpp"
#include "UniversalData.hpp"
#include "UniversalValue.hpp"
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

namespace nlohmann {
    void adl_serializer<UniversalData>::to_json(json& json, const UniversalData& universalData) {
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

    UniversalData adl_serializer<UniversalData>::from_json(const json& json) {
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
    }
} // namespace nlohmann

void to_json(nlohmann::json& json, AccountType accountType) { json = EnumToString<AccountType>(accountType); }

void from_json(const nlohmann::json& json, AccountType& accountType) { accountType = EnumFromString<AccountType>(json.get<std::string>()); }

void to_json(nlohmann::json& json, DeviceType deviceType) { json = EnumToString<DeviceType>(deviceType); }

void from_json(const nlohmann::json& json, DeviceType& deviceType) { deviceType = EnumFromString<DeviceType>(json.get<std::string>()); }

void to_json(nlohmann::json& json, const EventType eventType) { json = EnumToString<EventType>(eventType); }

void from_json(const nlohmann::json& json, EventType& eventType) { eventType = EnumFromString<EventType>(json.get<std::string>()); }

void to_json(nlohmann::json& json, Subject subject) { json = EnumToString<Subject>(subject); }

void from_json(const nlohmann::json& json, Subject& subject) { subject = EnumFromString<Subject>(json.get<std::string>()); }

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

void to_json(nlohmann::json& json, const DeviceDescription& deviceDescription) {
    json = {
        { "type", deviceDescription._type },
        { "id", deviceDescription._id },
    };
}

void from_json(const nlohmann::json& json, DeviceDescription& deviceDescription) {
    deviceDescription = DeviceDescription{
        ._type = EnumFromString<DeviceType>(json.at("type").get<std::string>()),
        ._id = json.at("id").get<Uuid>(),
    };
}

void to_json(nlohmann::json& json, const DeviceInformationDescription& deviceInformationDescription) {
    json = (const DeviceDescription&)deviceInformationDescription;
    json += { "seconds", deviceInformationDescription._seconds };
}

void from_json(const nlohmann::json& json, DeviceInformationDescription& deviceInformationDescription) {
    (DeviceDescription&)deviceInformationDescription = json.get<DeviceDescription>();
    deviceInformationDescription._seconds = json.value("seconds", (std::uint64_t)0);
}

void to_json(nlohmann::json& json, const DeviceProperty& deviceProperty) {
    json = {
        { "value", deviceProperty._value },
    };
}

void from_json(const nlohmann::json& json, DeviceProperty& deviceProperty) { deviceProperty._value = json.value("value", std::string()); }

void to_json(nlohmann::json& json, const BaseEvent& baseEvent) {
    json = {
        { "id", baseEvent._id.data() },      { "name", baseEvent._name },         { "active", baseEvent._active },
        { "provider", baseEvent._provider }, { "receiver", baseEvent._receiver },
    };
}

void from_json(const nlohmann::json& json, BaseEvent& baseEvent) {
    baseEvent._id = json["id"].get<Uuid>();
    baseEvent._name = json.value("name", "");
    baseEvent._active = json.value("active", true);
    baseEvent._provider = json["provider"].get<Provider>();
    baseEvent._receiver = json["receiver"].get<Receiver>();
}

void to_json(nlohmann::json& json, const Event& event) {
    json = std::visit([](const auto& e) -> nlohmann::json { return static_cast<nlohmann::json>(e); }, event);
}

void from_json(const nlohmann::json& json, Event& event) {
    const std::string type = json.value("type", std::string{});
    if (type == "timer_event") {
        event.emplace<TimerEvent>(json.get<TimerEvent>());
        return;
    }

    if (type == "thermometer_event") {
        event.emplace<ThermometerEvent>(json.get<ThermometerEvent>());
        return;
    }

    if (type == "relay_event") {
        event.emplace<RelayEvent>(json.get<RelayEvent>());
        return;
    }

    if (type == "thermostat_event") {
        event.emplace<ThermostatEvent>(json.get<ThermostatEvent>());
        return;
    }

    if (type == "sunrise_event") {
        event.emplace<SunriseEvent>(json.get<SunriseEvent>());
        return;
    }

    if (type == "sunset_event") {
        event.emplace<SunsetEvent>(json.get<SunsetEvent>());
        return;
    }

    LOG_ERROR_MSG(fmt::format("Invalid EventType: {}", type));
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
    messageHeader._description = json["description"].get<Provider>();
    // TODO: do we really have message without Subject?
    messageHeader._subject = json.contains("subject") ? json["subject"].get<Subject>() : Subject::Undefined;
}

void to_json(nlohmann::json& json, const MotionRelaySettings& motionRelaySettings) {
    json = (const PeriodSettings&)motionRelaySettings;
    json += { "activityTime", motionRelaySettings._activityTime };
}

void from_json(const nlohmann::json& json, MotionRelaySettings& motionRelaySettings) {
    (PeriodSettings&)motionRelaySettings = json.get<PeriodSettings>();
    motionRelaySettings._activityTime = json.value("activityTime", DEFAULT_ACTIVITY_TIME);
}

void to_json(nlohmann::json& json, const MotionRelayValue& motionRelayValue) {
    json = {
        { "motion", motionRelayValue._motion },
        { "state", motionRelayValue._state },
    };

    if (motionRelayValue._timestamp.has_value())
        json += { "timestamp", TimeHelper::TimeToInt(motionRelayValue._timestamp.value()) };
}

void from_json(const nlohmann::json& json, MotionRelayValue& motionRelayValue) {
    motionRelayValue._motion = json.value("motion", false);
    motionRelayValue._state = json.value("value", std::numeric_limits<int>::min());

    if (json.contains("timestamp"))
        motionRelayValue._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const PeriodSettings& periodSettings) {
    json = {
        { "period", periodSettings._period },
    };
}

void from_json(const nlohmann::json& json, PeriodSettings& periodSettings) { periodSettings._period = json.value("period", DEFAULT_PERIOD); }

void to_json(nlohmann::json& json, const Provider& provider) {
    std::visit(boost::hof::match([&json](const DeviceDescription& device) { json = device; },
                                 [&json](const ClientProvider& client) {
                                     json = {
                                         { "type", "client" },
                                         { "id", Constants::PredefinedIdClient },
                                     };
                                 },
                                 [&json](const EventProvider& event) {
                                     json = {
                                         { "type", "event" },
                                         { "id", event._id },
                                     };
                                 }),
               provider);
}

void from_json(const nlohmann::json& json, Provider& provider) {
    const std::string_view type = json.at("type").get<std::string_view>();
    if (type == "client")
        provider = ClientProvider{};
    else if (type == "event")
        provider = EventProvider{
            ._id = json.at("id").get<Uuid>(),
        };
    else
        provider = json.get<DeviceDescription>();
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

void to_json(nlohmann::json& json, const RelayEvent& relayEvent) {
    json = (const BaseEvent&)relayEvent;
    json += { "type", relayEvent._type };
    json += { "state", relayEvent._state };
    json += { "command", relayEvent._command };
}

void from_json(const nlohmann::json& json, RelayEvent& relayEvent) {
    (BaseEvent&)relayEvent = json.get<BaseEvent>();
    relayEvent._state = json.value("state", 0);
    relayEvent._command = json["command"].get<Command>();
}

void to_json(nlohmann::json& json, const RelayState& relayState) {
    json = {
        { "state", relayState._state },
    };
}

void from_json(const nlohmann::json& json, RelayState& relayState) { relayState._state = json.value("state", std::numeric_limits<int>::min()); }

void to_json(nlohmann::json& json, const RelayValue& relayValue) {
    json = {
        { "state", relayValue._state },
    };

    if (relayValue._timestamp.has_value())
        json += { "timestamp", TimeHelper::TimeToInt(relayValue._timestamp.value()) };
}

void from_json(const nlohmann::json& json, RelayValue& relayValue) {
    relayValue._state = json.value("state", std::numeric_limits<int>::min());

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

void to_json(nlohmann::json& json, const ThermometerEvent& thermometerEvent) {
    json = (const BaseEvent&)thermometerEvent;
    json += { "type", thermometerEvent._type };
    json += { "temperature", thermometerEvent._temperature };
    json += { "lower", thermometerEvent._lower };
    json += { "command", thermometerEvent._command };
}

void from_json(const nlohmann::json& json, ThermometerEvent& thermometerEvent) {
    (BaseEvent&)thermometerEvent = json.get<BaseEvent>();
    thermometerEvent._temperature = json.value("temperature", std::numeric_limits<float>::min());
    thermometerEvent._lower = json.value("lower", true);
    thermometerEvent._command = json["command"].get<Command>();
}

void to_json(nlohmann::json& json, const ThermostatEvent& thermostatEvent) {
    json = (const BaseEvent&)thermostatEvent;
    json += { "type", thermostatEvent._type };
    json += { "temperature", thermostatEvent._temperature };
    json += { "delta", thermostatEvent._delta };
}

void from_json(const nlohmann::json& json, ThermostatEvent& thermostatEvent) {
    (BaseEvent&)thermostatEvent = json.get<BaseEvent>();
    thermostatEvent._temperature = json.value("temperature", std::numeric_limits<float>::min());
    thermostatEvent._delta = json.value("delta", 0.5f);
}

void to_json(nlohmann::json& json, const TimerEvent& timerEvent) {
    json = (const BaseEvent&)timerEvent;
    json += { "type", timerEvent._type };
    json += { "hour", timerEvent._hour };
    json += { "minute", timerEvent._minute };
    json += { "command", timerEvent._command };
}

void from_json(const nlohmann::json& json, TimerEvent& timerEvent) {
    (BaseEvent&)timerEvent = json.get<BaseEvent>();
    timerEvent._hour = json.value("hour", 0);
    timerEvent._minute = json.value("minute", 0);
    timerEvent._command = json["command"].get<Command>();
}

void to_json(nlohmann::json& json, const SunriseEvent& sunriseEvent) {
    json = (const BaseEvent&)sunriseEvent;
    json += { "type", sunriseEvent._type };
    json += { "command", sunriseEvent._command };
}

void from_json(const nlohmann::json& json, SunriseEvent& sunriseEvent) {
    (BaseEvent&)sunriseEvent = json.get<BaseEvent>();
    sunriseEvent._command = json["command"].get<Command>();
}

void to_json(nlohmann::json& json, const SunsetEvent& sunsetEvent) {
    json = (const BaseEvent&)sunsetEvent;
    json += { "type", sunsetEvent._type };
    json += { "command", sunsetEvent._command };
}

void from_json(const nlohmann::json& json, SunsetEvent& sunsetEvent) {
    (BaseEvent&)sunsetEvent = json.get<BaseEvent>();
    sunsetEvent._command = json["command"].get<Command>();
}

void to_json(nlohmann::json& json, const UniversalValue& universalValue) {
    json = {
        { "values", universalValue._values },
    };

    if (universalValue._timestamp.has_value())
        json += { "timestamp", TimeHelper::TimeToInt(universalValue._timestamp.value()) };
}

void from_json(const nlohmann::json& json, UniversalValue& universalValue) {
    universalValue._values = json.at("values").get<std::map<std::string, UniversalData>>();

    if (json.contains("timestamp"))
        universalValue._timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
}

void to_json(nlohmann::json& json, const WebSocketAuthentication& webSocketAuthentication) {
    json = {
        { "authString", webSocketAuthentication._authString },
    };
}

void from_json(const nlohmann::json& json, WebSocketAuthentication& webSocketAuthentication) {
    webSocketAuthentication._authString = json.value("authString", "");
}
