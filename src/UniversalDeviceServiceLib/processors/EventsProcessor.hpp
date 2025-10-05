#pragma once

#include <vector>

#include "BaseProcessorWithQueryExecutor.hpp"
#include "CommandsController.hpp"
#include "CurrentTime.hpp"
#include "RelayEvent.hpp"
#include "SunriseEvent.hpp"
#include "SunsetEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermostatEvent.hpp"
#include "TimerEvent.hpp"

class EventsProcessor final : public BaseProcessorWithQueryExecutor {
public:
    EventsProcessor(IQueryExecutor* queryExecutor, CommandsController& commandsController);

    virtual ~EventsProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    std::vector<nlohmann::json> LoadEvents(const ComponentDescription& description);

    void ProcessTimerEvent(const TimerEvent& timeEvent, const Message& message);

    void ProcessThermometerEvent(const ThermometerEvent& thermometerEvent, const Message& message);

    void ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message);

    void ProcessThermostatEvent(const ThermostatEvent& thermostatEvent, const Message& message);

    struct SunriseSunsetTime {
        int day = -1;
        int month = -1;
        int sunriseHour = -1;
        int sunriseMinute = -1;
        int sunsetHour = -1;
        int sunsetMinute = -1;
    };

    void UpdateSunriseSunsetTime(const CurrentTime& currentTime) const;

    void ProcessSunriseEvent(const SunriseEvent& sunriseEvent, const Message& message);

    void ProcessSunsetEvent(const SunsetEvent& sunsetEvent, const Message& message);

    void SendCommand(const Uuid& id, const std::string& commandString, const std::string& logMessage);

private:
    static EventsProcessor::SunriseSunsetTime currentSunriseSunsetTime;

    CommandsController& _commandsController;
};
