#ifndef _EVENTS_PROCESSOR_H_
#define _EVENTS_PROCESSOR_H_

#include <vector>

#include "BaseProcessorWithQueryExecutor.h"
#include "Event.h"
#include "TimerEvent.h"
#include "ThermometerEvent.h"
#include "RelayEvent.h"
#include "ThermostatEvent.h"

class EventsProcessor final : public BaseProcessorWithQueryExecutor
{
public:
    EventsProcessor(IQueryExecutor* queryExecutor);

    virtual ~EventsProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    std::vector<nlohmann::json> LoadEvents(const ComponentDescription& description);

    void ProcessTimerEvent(const TimerEvent& timeEvent, const Message& message);

    void ProcessThermometerEvent(const ThermometerEvent& thermometerEvent, const Message& message);

    void ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message);

    void ProcessThermostatEvent(const ThermostatEvent& thermostatEvent, const Message& message);

    void SendCommand(const Uuid& id, const std::string& commandString);
};

#endif //_EVENTS_PROCESSOR_H_
