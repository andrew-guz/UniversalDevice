#pragma once

#include <chrono>
#include <string>

#include <nlohmann/json_fwd.hpp>

#include "Command.hpp"
#include "CommandsController.hpp"
#include "EventsController.hpp"
#include "IProcessor.hpp"
#include "Message.hpp"
#include "RelayEvent.hpp"
#include "SunriseEvent.hpp"
#include "SunriseSunsetUtils.hpp"
#include "SunsetEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermostatEvent.hpp"
#include "TimerEvent.hpp"
#include "Uuid.hpp"

class EventsProcessor final : public IProcessor {
public:
    EventsProcessor(EventsController& eventsController, CommandsController& commandsController);

    virtual ~EventsProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    void ProcessTimerEvent(std::chrono::system_clock::time_point timestamp, const TimerEvent& timeEvent);

    void ProcessThermometerEvent(const ThermometerEvent& thermometerEvent, const Message& message);

    void ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message);

    void ProcessThermostatEvent(const ThermostatEvent& thermostatEvent, const Message& message);

    void UpdateSunriseSunsetTime(std::chrono::system_clock::time_point timestamp) const;

    void ProcessSunriseEvent(std::chrono::system_clock::time_point timestamp, const SunriseEvent& sunriseEvent);

    void ProcessSunsetEvent(std::chrono::system_clock::time_point timestamp, const SunsetEvent& sunsetEvent);

    void SendCommand(const Uuid& id, const Command& command, const std::string& logMessage);

private:
    static SunriseSunsetTime currentSunriseSunsetTime;

    EventsController& _eventsController;

    CommandsController& _commandsController;
};
