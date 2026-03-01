#include "EventsProcessor.hpp"

#include <chrono>
#include <cmath>
#include <limits>
#include <optional>
#include <string>
#include <tuple>
#include <variant>

#include <boost/hof.hpp>
#include <boost/hof/match.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Command.hpp"
#include "CommandsController.hpp"
#include "Event.hpp"
#include "EventsController.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Provider.hpp"
#include "RelayState.hpp"
#include "RelayValue.hpp"
#include "SunriseEvent.hpp"
#include "SunriseSunsetUtils.hpp"
#include "ThermometerValue.hpp"
#include "TimeHelper.hpp"
#include "WebsocketsCache.hpp"

SunriseSunsetTime EventsProcessor::currentSunriseSunsetTime;

EventsProcessor::EventsProcessor(EventsController& eventsController, CommandsController& commandsController) :
    _eventsController(eventsController),
    _commandsController(commandsController) {}

nlohmann::json EventsProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    for (const Event& event : _eventsController.GetByProvider(GetProviderId(message._header._description))) {
        std::visit(boost::hof::match([this, timestamp, &message](const TimerEvent& e) { ProcessTimerEvent(timestamp, e); },
                                     [this, &message](const ThermometerEvent& e) { ProcessThermometerEvent(e, message); },
                                     [this, &message](const RelayEvent& e) { ProcessRelayEvent(e, message); },
                                     [this, &message](const ThermostatEvent& e) { ProcessThermostatEvent(e, message); },
                                     [this, timestamp, &message](const SunriseEvent& e) { ProcessSunriseEvent(timestamp, e); },
                                     [this, timestamp, &message](const SunsetEvent& e) { ProcessSunsetEvent(timestamp, e); }),
                   event);
    }
    return {};
}

void EventsProcessor::ProcessTimerEvent(const std::chrono::system_clock::time_point timestamp, const TimerEvent& timeEvent) {
    auto [hour, minute] = TimeHelper::GetHourMinute(timestamp);
    if (hour == timeEvent._hour && minute == timeEvent._minute)
        SendCommand(timeEvent._receiver._id, timeEvent._command, fmt::format("Инициировано временное событие `{}`", timeEvent._name));
}

void EventsProcessor::ProcessThermometerEvent(const ThermometerEvent& thermometerEvent, const Message& message) {
    auto thermometerCurrentValue = message._data.get<ThermometerValue>();
    if (thermometerCurrentValue._value == std::numeric_limits<int>::min()) {
        LOG_ERROR_MSG("Invalid value in EventsProcessor::ProcessThermometerEvent");
        return;
    }

    if (std::abs(thermometerCurrentValue._value - ThermometerValue::InvalidTemperature) < 0.1f) {
        return;
    }

    if ((thermometerEvent._lower == true && thermometerCurrentValue._value <= thermometerEvent._temperature) ||
        (thermometerEvent._lower == false && thermometerCurrentValue._value >= thermometerEvent._temperature))
        SendCommand(
            thermometerEvent._receiver._id, thermometerEvent._command, fmt::format("Инициировано событие термометра `{}`", thermometerEvent._name));
}

void EventsProcessor::ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message) {
    auto relayCurrentState = message._data.get<RelayValue>();
    if (relayCurrentState._state == std::numeric_limits<int>::min()) {
        LOG_ERROR_MSG("Invalid value in EventsProcessor::ProcessRelayEvent");
        return;
    }

    if (relayCurrentState._state == relayEvent._state)
        SendCommand(relayEvent._receiver._id, relayEvent._command, fmt::format("Инициировано событие реле `{}`", relayEvent._name));
}

void EventsProcessor::ProcessThermostatEvent(const ThermostatEvent& thermostatEvent, const Message& message) {
    auto thermometerCurrentValue = message._data.get<ThermometerValue>();
    if (thermometerCurrentValue._value == std::numeric_limits<int>::min()) {
        LOG_ERROR_MSG("Invalid value in EventsProcessor::ProcessThermostatEvent");
        return;
    }

    if (std::abs(thermometerCurrentValue._value - ThermometerValue::InvalidTemperature) < 0.1f) {
        return;
    }

    if (thermometerCurrentValue._value <= thermostatEvent._temperature - thermostatEvent._delta) {
        // on
        RelayState relayState;
        relayState._state = true;
        SendCommand(thermostatEvent._receiver._id, relayState, fmt::format("Инициировано включения реле термостатом `{}`", thermostatEvent._name));
    }

    if (thermometerCurrentValue._value >= thermostatEvent._temperature + thermostatEvent._delta) {
        // off
        RelayState relayState;
        relayState._state = false;
        SendCommand(thermostatEvent._receiver._id, relayState, fmt::format("Инициировано выключения реле термостатом `{}`", thermostatEvent._name));
    }
}

void EventsProcessor::UpdateSunriseSunsetTime(const std::chrono::system_clock::time_point timestamp) const {
    int day;
    int month;
    int year;
    std::tie(day, month, year) = TimeHelper::GetDayMonthYear(timestamp);
    if (EventsProcessor::currentSunriseSunsetTime.day == day && EventsProcessor::currentSunriseSunsetTime.month == month + 1)
        return;

    EventsProcessor::currentSunriseSunsetTime = getSunriseSunsetTime(day, month, year);
}

void EventsProcessor::ProcessSunriseEvent(const std::chrono::system_clock::time_point timestamp, const SunriseEvent& sunriseEvent) {
    UpdateSunriseSunsetTime(timestamp);
    auto [hour, minute] = TimeHelper::GetHourMinute(timestamp);
    if (hour == EventsProcessor::currentSunriseSunsetTime.sunriseHour && minute == EventsProcessor::currentSunriseSunsetTime.sunriseMinute)
        SendCommand(sunriseEvent._receiver._id, sunriseEvent._command, fmt::format("Инициировано событие восхода `{}`", sunriseEvent._name));
}

void EventsProcessor::ProcessSunsetEvent(const std::chrono::system_clock::time_point timestamp, const SunsetEvent& sunsetEvent) {
    UpdateSunriseSunsetTime(timestamp);
    auto [hour, minute] = TimeHelper::GetHourMinute(timestamp);
    if (hour == EventsProcessor::currentSunriseSunsetTime.sunsetHour && minute == EventsProcessor::currentSunriseSunsetTime.sunsetMinute)
        SendCommand(sunsetEvent._receiver._id, sunsetEvent._command, fmt::format("Инициировано событие заката `{}`", sunsetEvent._name));
}

void EventsProcessor::SendCommand(const Uuid& id, const Command& command, const std::string& logMessage) {
    try {
        const std::optional<Command> existingCommand = _commandsController.Get(id);

        if (existingCommand.has_value() && existingCommand.value() == command)
            return;

        if (_commandsController.AddOrUpdate(id, command)) {
            auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(id);
            if (connection) {
                connection->send_text(static_cast<nlohmann::json>(command).dump());
                LOG_INFO_MSG(logMessage);
            }
        }
    } catch (...) {
        LOG_ERROR_MSG(fmt::format("EventsProcessor::SendCommand '{}' failed", static_cast<nlohmann::json>(command).dump()));
    }
}
