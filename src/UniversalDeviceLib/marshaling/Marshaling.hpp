#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Command.hpp"
#include "DeviceDescription.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Provider.hpp"
#include "Settings.hpp"
#include "UniversalData.hpp"

class Uuid;
struct Account;
struct BaseEvent;
struct Device;
struct DeviceInformationDescription;
struct DeviceProperty;
struct LogInformation;
struct Message;
struct MessageHeader;
struct MotionRelaySettings;
struct MotionRelayValue;
struct PeriodSettings;
struct RelayEvent;
struct RelayState;
struct RelayValue;
struct Scenario;
struct ThermometerEvent;
struct ThermometerLedBrightness;
struct ThermometerValue;
struct ThermostatEvent;
struct TimerEvent;
struct SunriseEvent;
struct SunsetEvent;
struct UniversalValue;
struct WebSocketAuthentication;

template<typename EnumType>
std::string EnumToString(EnumType enumType) = delete;

template<typename EnumType>
EnumType EnumFromString(const std::string& str) = delete;

template<>
std::string EnumToString(AccountType enumType);

template<>
AccountType EnumFromString(const std::string& str);

template<>
std::string EnumToString(DeviceType enumType);

template<>
DeviceType EnumFromString(const std::string& str);

template<>
std::string EnumToString(EventType enumType);

template<>
EventType EnumFromString(const std::string& str);

template<>
LogLevel EnumFromString(const std::string& str);

template<>
std::string EnumToString(Subject enumType);

template<>
Subject EnumFromString(const std::string& str);

template<>
std::string EnumToString(UniversalDataType enumType);

template<>
UniversalDataType EnumFromString(const std::string& str);

namespace nlohmann {
    template<>
    struct adl_serializer<UniversalData> {
        static void to_json(json& json, const UniversalData& universalData);

        static UniversalData from_json(const json& json);
    };

    template<typename T>
    struct adl_serializer<std::optional<T>> {
        static void to_json(json& json, const std::optional<T>& optionalData) {
            if (optionalData.has_value()) {
                json = optionalData.value();
            } else {
                json = nullptr;
            }
        }

        static std::optional<T> from_json(const json& json) {
            if (json.is_null()) {
                return std::nullopt;
            } else {
                return json.get<T>();
            }
        }
    };
} // namespace nlohmann

void to_json(nlohmann::json& json, AccountType accountType);

void from_json(const nlohmann::json& json, AccountType& accountType);

void to_json(nlohmann::json& json, DeviceType deviceType);

void from_json(const nlohmann::json& json, DeviceType& deviceType);

void to_json(nlohmann::json& json, EventType eventType);

void from_json(const nlohmann::json& json, EventType& eventType);

void to_json(nlohmann::json& json, Subject subject);

void from_json(const nlohmann::json& json, Subject& subject);

void to_json(nlohmann::json& json, const Uuid& uuid);

void from_json(const nlohmann::json& json, Uuid& uuid);

void to_json(nlohmann::json& json, const Account& account);

void from_json(const nlohmann::json& json, Account& account);

void to_json(nlohmann::json& json, const Device& device);

void from_json(const nlohmann::json& json, Device& device);

void to_json(nlohmann::json& json, const DeviceDescription& deviceDescription);

void from_json(const nlohmann::json& json, DeviceDescription& deviceDescription);

void to_json(nlohmann::json& json, const DeviceInformationDescription& deviceInformationDescription);

void from_json(const nlohmann::json& json, DeviceInformationDescription& deviceInformationDescription);

void to_json(nlohmann::json& json, const DeviceProperty& deviceProperty);

void from_json(const nlohmann::json& json, DeviceProperty& deviceProperty);

void to_json(nlohmann::json& json, const BaseEvent& baseEvent);

void from_json(const nlohmann::json& json, BaseEvent& baseEvent);

void to_json(nlohmann::json& json, const Event& event);

void from_json(const nlohmann::json& json, Event& event);

void to_json(nlohmann::json& json, const LogInformation& logInformation);

void from_json(const nlohmann::json& json, LogInformation& logInformation);

void from_json(const nlohmann::json& json, LogLevel& logLevel);

void to_json(nlohmann::json& json, const Message& message);

void from_json(const nlohmann::json& json, Message& message);

void to_json(nlohmann::json& json, const MessageHeader& messageHeader);

void from_json(const nlohmann::json& json, MessageHeader& messageHeader);

void to_json(nlohmann::json& json, const MotionRelaySettings& motionRelaySettings);

void from_json(const nlohmann::json& json, MotionRelaySettings& motionRelaySettings);

void to_json(nlohmann::json& json, const MotionRelayValue& motionRelayValue);

void from_json(const nlohmann::json& json, MotionRelayValue& motionRelayValue);

void to_json(nlohmann::json& json, const PeriodSettings& periodSettings);

void from_json(const nlohmann::json& json, PeriodSettings& periodSettings);

void to_json(nlohmann::json& json, const Provider& provider);

void from_json(const nlohmann::json& json, Provider& provider);

void to_json(nlohmann::json& json, const Settings& settings);

void from_json(const nlohmann::json& json, Settings& settings);

void to_json(nlohmann::json& json, const RelayEvent& relayEvent);

void from_json(const nlohmann::json& json, RelayEvent& relayEvent);

void to_json(nlohmann::json& json, const RelayState& relayState);

void from_json(const nlohmann::json& json, RelayState& relayState);

void to_json(nlohmann::json& json, const RelayValue& relayValue);

void from_json(const nlohmann::json& json, RelayValue& relayValue);

void to_json(nlohmann::json& json, const Scenario& scenario);

void from_json(const nlohmann::json& json, Scenario& scenario);

void to_json(nlohmann::json& json, const ThermometerLedBrightness& thermometerLedBrightness);

void from_json(const nlohmann::json& json, ThermometerLedBrightness& thermometerLedBrightness);

void to_json(nlohmann::json& json, const Command& command);

void from_json(const nlohmann::json& json, Command& command);

void to_json(nlohmann::json& json, const ThermometerValue& thermometerValue);

void from_json(const nlohmann::json& json, ThermometerValue& thermometerValue);

void to_json(nlohmann::json& json, const ThermometerEvent& thermometerEvent);

void from_json(const nlohmann::json& json, ThermometerEvent& thermometerEvent);

void to_json(nlohmann::json& json, const ThermostatEvent& thermostatEvent);

void from_json(const nlohmann::json& json, ThermostatEvent& thermostatEvent);

void to_json(nlohmann::json& json, const TimerEvent& timerEvent);

void from_json(const nlohmann::json& json, TimerEvent& timerEvent);

void to_json(nlohmann::json& json, const SunriseEvent& sunriseEvent);

void from_json(const nlohmann::json& json, SunriseEvent& sunriseEvent);

void to_json(nlohmann::json& json, const SunsetEvent& sunsetEvent);

void from_json(const nlohmann::json& json, SunsetEvent& sunsetEvent);

void to_json(nlohmann::json& json, const UniversalValue& universalValue);

void from_json(const nlohmann::json& json, UniversalValue& universalValue);

void to_json(nlohmann::json& json, const WebSocketAuthentication& webSocketAuthentication);

void from_json(const nlohmann::json& json, WebSocketAuthentication& webSocketAuthentication);
