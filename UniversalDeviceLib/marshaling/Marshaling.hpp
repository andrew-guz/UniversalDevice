#pragma once

#include "Enums.hpp"
#include "Types.hpp"

#include <nlohmann/json.hpp>

class Uuid;
struct Account;
struct ComponentDescription;
struct CurrentTime;
struct DeviceInformationDescription;
struct DeviceProperty;
struct Event;
struct ExtendedComponentDescription;
struct ExtendedMotionRelayCurrentState;
struct ExtendedRelayCurrentState;
struct ExtendedThermometerCurrentValue;
struct LogInformation;
struct Message;
struct MessageHeader;
struct MotionRelayCurrentState;
struct MotionRelaySettings;
struct PeriodSettings;
struct RelayCurrentState;
struct RelayEvent;
struct RelayState;
struct ThermometerCurrentValue;
struct ThermometerEvent;
struct ThermometerLedBrightness;
struct ThermostatEvent;
struct TimerEvent;
struct WebSocketAuthentication;

template<typename EnumType>
std::string EnumToString(EnumType enumType) = delete;

template<typename EnumType>
EnumType EnumFromString(const std::string& str) = delete;

template<>
std::string EnumToString(DeviceType enumType);

template<>
DeviceType EnumFromString(const std::string& str);

template<>
std::string EnumToString(EventType enumType);

template<>
EventType EnumFromString(const std::string& str);

std::string ActorTypeToString(const ActorType& type);

ActorType ActorTypeFromString(const std::string& str);

void to_json(nlohmann::json& json, EventType eventType);

void from_json(const nlohmann::json& json, EventType& eventType);

void to_json(nlohmann::json& json, DeviceType deviceType);

void from_json(const nlohmann::json& json, DeviceType& deviceType);

void to_json(nlohmann::json& json, const ActorType& type);

void from_json(const nlohmann::json& json, ActorType& type);

void to_json(nlohmann::json& json, const Uuid& uuid);

void from_json(const nlohmann::json& json, Uuid& uuid);

void to_json(nlohmann::json& json, const Account& account);

void from_json(const nlohmann::json& json, Account& account);

void to_json(nlohmann::json& json, const ComponentDescription& componentDescription);

void from_json(const nlohmann::json& json, ComponentDescription& componentDescription);

void to_json(nlohmann::json& json, const CurrentTime& currentTime);

void from_json(const nlohmann::json& json, CurrentTime& currentTime);

void to_json(nlohmann::json& json, const DeviceInformationDescription& deviceInformationDescription);

void from_json(const nlohmann::json& json, DeviceInformationDescription& deviceInformationDescription);

void to_json(nlohmann::json& json, const DeviceProperty& deviceProperty);

void from_json(const nlohmann::json& json, DeviceProperty& deviceProperty);

void to_json(nlohmann::json& json, const Event& event);

void from_json(const nlohmann::json& json, Event& event);

void to_json(nlohmann::json& json, const ExtendedComponentDescription& extendedComponentDescription);

void from_json(const nlohmann::json& json, ExtendedComponentDescription& extendedComponentDescription);

void to_json(nlohmann::json& json, const ExtendedMotionRelayCurrentState& extendedMotionRelayCurrentState);

void from_json(const nlohmann::json& json, ExtendedMotionRelayCurrentState& extendedMotionRelayCurrentState);

void to_json(nlohmann::json& json, const ExtendedRelayCurrentState& extendedRelayCurrentState);

void from_json(const nlohmann::json& json, ExtendedRelayCurrentState& extendedRelayCurrentState);

void to_json(nlohmann::json& json, const ExtendedThermometerCurrentValue& extendedThermometerCurrentValue);

void from_json(const nlohmann::json& json, ExtendedThermometerCurrentValue& extendedThermometerCurrentValue);

void to_json(nlohmann::json& json, const LogInformation& logInformation);

void from_json(const nlohmann::json& json, LogInformation& logInformation);

void to_json(nlohmann::json& json, const Message& message);

void from_json(const nlohmann::json& json, Message& message);

void to_json(nlohmann::json& json, const MessageHeader& messageHeader);

void from_json(const nlohmann::json& json, MessageHeader& messageHeader);

void to_json(nlohmann::json& json, const MotionRelayCurrentState& motionRelayCurrentState);

void from_json(const nlohmann::json& json, MotionRelayCurrentState& motionRelayCurrentState);

void to_json(nlohmann::json& json, const MotionRelaySettings& motionRelaySettings);

void from_json(const nlohmann::json& json, MotionRelaySettings& motionRelaySettings);

void to_json(nlohmann::json& json, const PeriodSettings& periodSettings);

void from_json(const nlohmann::json& json, PeriodSettings& periodSettings);

void to_json(nlohmann::json& json, const RelayCurrentState& relayCurrentState);

void from_json(const nlohmann::json& json, RelayCurrentState& relayCurrentState);

void to_json(nlohmann::json& json, const RelayEvent& relayEvent);

void from_json(const nlohmann::json& json, RelayEvent& relayEvent);

void to_json(nlohmann::json& json, const RelayState& relayState);

void from_json(const nlohmann::json& json, RelayState& relayState);

void to_json(nlohmann::json& json, const ThermometerLedBrightness& thermometerLedBrightness);

void from_json(const nlohmann::json& json, ThermometerLedBrightness& thermometerLedBrightness);

void to_json(nlohmann::json& json, const ThermometerCurrentValue& thermometerCurrentValue);

void from_json(const nlohmann::json& json, ThermometerCurrentValue& thermometerCurrentValue);

void to_json(nlohmann::json& json, const ThermometerEvent& thermometerEvent);

void from_json(const nlohmann::json& json, ThermometerEvent& thermometerEvent);

void to_json(nlohmann::json& json, const ThermostatEvent& thermostatEvent);

void from_json(const nlohmann::json& json, ThermostatEvent& thermostatEvent);

void to_json(nlohmann::json& json, const TimerEvent& timerEvent);

void from_json(const nlohmann::json& json, TimerEvent& timerEvent);

void to_json(nlohmann::json& json, const WebSocketAuthentication& webSocketAuthentication);

void from_json(const nlohmann::json& json, WebSocketAuthentication& webSocketAuthentication);
