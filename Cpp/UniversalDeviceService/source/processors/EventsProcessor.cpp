#include "EventsProcessor.h"

#include <sstream>

#include "IDb.h"
#include "Logger.h"
#include "TimeHelper.h"
#include "CurrentTime.h"
#include "ThermometerCurrentValue.h"
#include "RelayCurrentState.h"

EventsProcessor::EventsProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json EventsProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    //load all events
    std::vector<nlohmann::json> eventJsons = LoadEvents(message._header._description);
    //do events
    for (auto& eventJson : eventJsons)
    {       
        auto simpleEvent = JsonExtension::CreateFromJson<Event>(eventJson);
        if (simpleEvent._type == Constants::EventTypeTime)
        {
            auto timerEvent = JsonExtension::CreateFromJson<TimerEvent>(eventJson);
            ProcessTimerEvent(timerEvent, message);
        }
        else if (simpleEvent._type == Constants::EventTypeThermometer)
        {
            auto thermometerEvent = JsonExtension::CreateFromJson<ThermometerEvent>(eventJson);
            ProcessThermometerEvent(thermometerEvent, message);
        }
        else if (simpleEvent._type == Constants::EventTypeRelay)
        {
            auto relayEvent = JsonExtension::CreateFromJson<RelayEvent>(eventJson);
            ProcessRelayEvent(relayEvent, message);
        }
    }
    return {};
}

std::vector<nlohmann::json> EventsProcessor::LoadEvents(const ComponentDescription& description)
{
    std::vector<nlohmann::json> result;
    std::stringstream queryStream;
    queryStream
        << "SELECT event FROM Events WHERE providerId = '"
        << description._id.data()
        << "' AND providerType = '"
        << description._type        
        << "' AND active = 1";
    queryStream.flush();   
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(queryStream.str(), data))
    {
        for (auto& row : data)
        {
            auto eventString = DbExtension::FindValueByName(row, "event");
            if (eventString.size())
            {
                try
                {
                    auto json = nlohmann::json::parse(eventString);
                    result.push_back(json);
                }
                catch(...)
                {
                    LOG_ERROR << "Invalid JSON - " << eventString << "." << std::endl;
                }
            }
        }
    }
    else
        LOG_SQL_ERROR(queryStream.str());
    return result;
}

void EventsProcessor::ProcessTimerEvent(const TimerEvent& timeEvent, const Message& message)
{
    auto currentTime = JsonExtension::CreateFromJson<CurrentTime>(message._data);
    auto[hour, minute] = TimeHelper::GetHourMinute(currentTime._timestamp);
    if (hour == timeEvent._hour &&
        minute == timeEvent._minute)
        SendCommand(timeEvent._receiver._id, timeEvent._command.dump());
}

void EventsProcessor::ProcessThermometerEvent(const ThermometerEvent& thermometerEvent, const Message& message)
{
    auto thermometerCurrentValue = JsonExtension::CreateFromJson<ThermometerCurrentValue>(message._data);
    if (thermometerCurrentValue._value == std::numeric_limits<int>::min())
    {
        LOG_ERROR << "Invalid value in EventsProcessor::ProcessThermometerEvent." << std::endl;
        return;
    }
    if ((thermometerEvent._lower == true &&
         thermometerCurrentValue._value <= thermometerEvent._temperature) ||
        (thermometerEvent._lower == false &&
         thermometerCurrentValue._value >= thermometerEvent._temperature))
        SendCommand(thermometerEvent._receiver._id, thermometerEvent._command.dump());
}

void EventsProcessor::ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message)
{
    auto relayCurrentState = JsonExtension::CreateFromJson<RelayCurrentState>(message._data);
    if (relayCurrentState._state == std::numeric_limits<int>::min())
    {
        LOG_ERROR << "Invalid value in EventsProcessor::ProcessRelayEvent." << std::endl;
        return;
    }
    if (relayCurrentState._state == relayEvent._state)
        SendCommand(relayEvent._receiver._id, relayEvent._command.dump());
}

void EventsProcessor::SendCommand(const Uuid& id, const std::string& commandString)
{
    std::stringstream queryStream;
    queryStream << "INSERT OR REPLACE INTO Commands (id, commands) VALUES ('"
        << id.data()
        << "', '"
        << commandString
        << "')";
    queryStream.flush();
    if (!_queryExecutor->Execute(queryStream.str()))
        LOG_SQL_ERROR(queryStream.str());
}
