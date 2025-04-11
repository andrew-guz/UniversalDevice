#include "EventsProcessor.hpp"

#include <future>

#include "nlohmann/json_fwd.hpp"
#include <fmt/format.h>

#include "CurrentTime.hpp"
#include "Enums.hpp"
#include "EventTableStorageCache.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayCurrentState.hpp"
#include "RelayState.hpp"
#include "RequestHelper.hpp"
#include "SimpleTableStorageCache.hpp"
#include "SunriseEvent.hpp"
#include "ThermometerCurrentValue.hpp"
#include "TimeHelper.hpp"
#include "WebsocketsCache.hpp"

EventsProcessor::SunriseSunsetTime EventsProcessor::currentSunriseSunsetTime;

EventsProcessor::EventsProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json EventsProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    // load all events
    std::vector<nlohmann::json> eventJsons = LoadEvents(message._header._description);
    // do events
    for (auto& eventJson : eventJsons) {
        auto simpleEvent = eventJson.get<Event>();
        switch (simpleEvent._type) {
            case EventType::Undefined:
                LOG_ERROR_MSG("Invalid event type");
                break;
            case EventType::Timer:
                ProcessTimerEvent(eventJson.get<TimerEvent>(), message);
                break;
            case EventType::Thermometer:
                ProcessThermometerEvent(eventJson.get<ThermometerEvent>(), message);
                break;
            case EventType::Relay:
                ProcessRelayEvent(eventJson.get<RelayEvent>(), message);
                break;
            case EventType::Thermostat:
                ProcessThermostatEvent(eventJson.get<ThermostatEvent>(), message);
                break;
            case EventType::Sunrise:
                ProcessSunriseEvent(eventJson.get<SunriseEvent>(), message);
                break;
            case EventType::Sunset:
                ProcessSunsetEvent(eventJson.get<SunsetEvent>(), message);
                break;
        }
    }
    return {};
}

std::vector<nlohmann::json> EventsProcessor::LoadEvents(const ComponentDescription& description) {
    std::vector<nlohmann::json> result;
    std::vector<std::string> eventStrings;

    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableSelectInput what{
        ._id = description._id,
        ._type = description._type,
    };
    EventTableSelectOutput eventsResult;
    auto problem = storageCache->Select(what, eventsResult);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            eventStrings = eventsResult._data;
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }

    for (auto& eventString : eventStrings) {
        try {
            auto json = nlohmann::json::parse(eventString);
            result.push_back(json);
        } catch (...) {
            LOG_ERROR_MSG(fmt::format("Invalid JSON - {}", eventString));
        }
    }

    return result;
}

void EventsProcessor::ProcessTimerEvent(const TimerEvent& timeEvent, const Message& message) {
    auto currentTime = message._data.get<CurrentTime>();
    auto [hour, minute] = TimeHelper::GetHourMinute(currentTime._timestamp);
    if (hour == timeEvent._hour && minute == timeEvent._minute)
        SendCommand(timeEvent._receiver._id, timeEvent._command.dump());
}

void EventsProcessor::ProcessThermometerEvent(const ThermometerEvent& thermometerEvent, const Message& message) {
    auto thermometerCurrentValue = message._data.get<ThermometerCurrentValue>();
    if (thermometerCurrentValue._value == std::numeric_limits<int>::min()) {
        LOG_ERROR_MSG("Invalid value in EventsProcessor::ProcessThermometerEvent");
        return;
    }
    if (std::abs(thermometerCurrentValue._value - ThermometerCurrentValue::InvalidTemperature) < 0.1f) {
        return;
    }
    if ((thermometerEvent._lower == true && thermometerCurrentValue._value <= thermometerEvent._temperature) ||
        (thermometerEvent._lower == false && thermometerCurrentValue._value >= thermometerEvent._temperature))
        SendCommand(thermometerEvent._receiver._id, thermometerEvent._command.dump());
}

void EventsProcessor::ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message) {
    auto relayCurrentState = message._data.get<RelayCurrentState>();
    if (relayCurrentState._state == std::numeric_limits<int>::min()) {
        LOG_ERROR_MSG("Invalid value in EventsProcessor::ProcessRelayEvent");
        return;
    }
    if (relayCurrentState._state == relayEvent._state)
        SendCommand(relayEvent._receiver._id, relayEvent._command.dump());
}

void EventsProcessor::ProcessThermostatEvent(const ThermostatEvent& thermostatEvent, const Message& message) {
    auto thermometerCurrentValue = message._data.get<ThermometerCurrentValue>();
    if (thermometerCurrentValue._value == std::numeric_limits<int>::min()) {
        LOG_ERROR_MSG("Invalid value in EventsProcessor::ProcessThermostatEvent");
        return;
    }
    if (std::abs(thermometerCurrentValue._value - ThermometerCurrentValue::InvalidTemperature) < 0.1f) {
        return;
    }
    std::string command;
    if (thermometerCurrentValue._value <= thermostatEvent._temperature - thermostatEvent._delta) {
        // on
        RelayState relayState;
        relayState._state = true;
        command = nlohmann::json(relayState).dump();
    }
    if (thermometerCurrentValue._value >= thermostatEvent._temperature + thermostatEvent._delta) {
        // off
        RelayState relayState;
        relayState._state = false;
        command = nlohmann::json(relayState).dump();
    }
    if (command.size())
        SendCommand(thermostatEvent._receiver._id, command);
}

void EventsProcessor::UpdateSunriseSunsetTime(const CurrentTime& currentTime) const {
    int day;
    int month;
    int year;
    std::tie(day, month, year) = TimeHelper::GetDayMonthYear(currentTime._timestamp);
    if (EventsProcessor::currentSunriseSunsetTime.day == day && EventsProcessor::currentSunriseSunsetTime.month == month + 1)
        return;

    // TODO: Use location
    [[maybe_unused]] auto future = std::async(std::launch::async, []() {
        auto [statusCode, data] = RequestHelper::DoGetOutsizeRequest("https://api.sunrisesunset.io/json?lat=59.945673&lng=30.342361&time_format=24");
        if (statusCode != 200)
            return;

        try {
            const nlohmann::json json = nlohmann::json::parse(data);
            if (json.at("status").get<std::string>() != "OK")
                return;

            const nlohmann::json results = json.at("results");
            const std::chrono::system_clock::time_point date = TimeHelper::TimeFromString(results.at("date").get<std::string>(), "%Y-%m-%d");
            int day;
            int month;
            int year;
            std::tie(day, month, year) = TimeHelper::GetDayMonthYear(date);
            EventsProcessor::currentSunriseSunsetTime.day = day;
            EventsProcessor::currentSunriseSunsetTime.month = month + 1;
            const std::chrono::system_clock::time_point sunrise = TimeHelper::TimeFromString(results.at("sunrise").get<std::string>(), "%H:%M:%S");
            std::tie(EventsProcessor::currentSunriseSunsetTime.sunriseHour, EventsProcessor::currentSunriseSunsetTime.sunriseMinute) =
                TimeHelper::GetHourMinute(sunrise);
            const std::chrono::system_clock::time_point sunset = TimeHelper::TimeFromString(results.at("sunset").get<std::string>(), "%H:%M:%S");
            std::tie(EventsProcessor::currentSunriseSunsetTime.sunsetHour, EventsProcessor::currentSunriseSunsetTime.sunsetMinute) =
                TimeHelper::GetHourMinute(sunset);
        } catch (...) {
            return;
        }
    });
}

void EventsProcessor::ProcessSunriseEvent(const SunriseEvent& sunriseEvent, const Message& message) {
    auto currentTime = message._data.get<CurrentTime>();
    UpdateSunriseSunsetTime(currentTime);
    auto [hour, minute] = TimeHelper::GetHourMinute(currentTime._timestamp);
    if (hour == EventsProcessor::currentSunriseSunsetTime.sunriseHour && minute == EventsProcessor::currentSunriseSunsetTime.sunriseMinute)
        SendCommand(sunriseEvent._receiver._id, sunriseEvent._command.dump());
}

void EventsProcessor::ProcessSunsetEvent(const SunsetEvent& sunsetEvent, const Message& message) {
    auto currentTime = message._data.get<CurrentTime>();
    UpdateSunriseSunsetTime(currentTime);
    auto [hour, minute] = TimeHelper::GetHourMinute(currentTime._timestamp);
    if (hour == EventsProcessor::currentSunriseSunsetTime.sunsetHour && minute == EventsProcessor::currentSunriseSunsetTime.sunsetMinute)
        SendCommand(sunsetEvent._receiver._id, sunsetEvent._command.dump());
}

void EventsProcessor::SendCommand(const Uuid& id, const std::string& commandString) {
    try {
        auto storageCache = GetCommandsCache(_queryExecutor);
        SimpleTableInsertOrReplaceInput what{
            ._id = id,
            ._data = commandString,
        };
        auto problem = storageCache->InsertOrReplace(what);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems: {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(id);
                if (connection)
                    connection->send_text(commandString);
            } break;
            case StorageCacheProblemType::Empty:
                LOG_ERROR_MSG(fmt::format("Invalid command {}", commandString));
                break;
            case StorageCacheProblemType::NotExists:
                break;
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("EventsProcessor::SendCommand");
    }
}
