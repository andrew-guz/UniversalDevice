#include "EventsProcessor.hpp"

#include <sstream>

#include "CurrentTime.hpp"
#include "DbExtension.hpp"
#include "Enums.hpp"
#include "EventTableStorageCache.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayCurrentState.hpp"
#include "RelayState.hpp"
#include "SimpleTableStorageCache.hpp"
#include "StorageCacheFactory.hpp"
#include "ThermometerCurrentValue.hpp"
#include "TimeHelper.hpp"
#include "WebsocketsCache.hpp"

EventsProcessor::EventsProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json EventsProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    // load all events
    std::vector<nlohmann::json> eventJsons = LoadEvents(message._header._description);
    // do events
    for (auto& eventJson : eventJsons) {
        auto simpleEvent = eventJson.get<Event>();
        switch (simpleEvent._type) {
            case EventType::Undefined:
                LOG_ERROR << "Invalid event type" << std::endl;
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
        }
    }
    return {};
}

std::vector<nlohmann::json> EventsProcessor::LoadEvents(const ComponentDescription& description) {
    std::vector<nlohmann::json> result;
    std::vector<std::string> eventStrings;

    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableSelectInput what;
    what._id = description._id.data();
    what._type = description._type;
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
            LOG_ERROR << "Invalid JSON - " << eventString << "." << std::endl;
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
        LOG_ERROR << "Invalid value in EventsProcessor::ProcessThermometerEvent." << std::endl;
        return;
    }
    if ((thermometerEvent._lower == true && thermometerCurrentValue._value <= thermometerEvent._temperature) ||
        (thermometerEvent._lower == false && thermometerCurrentValue._value >= thermometerEvent._temperature))
        SendCommand(thermometerEvent._receiver._id, thermometerEvent._command.dump());
}

void EventsProcessor::ProcessRelayEvent(const RelayEvent& relayEvent, const Message& message) {
    auto relayCurrentState = message._data.get<RelayCurrentState>();
    if (relayCurrentState._state == std::numeric_limits<int>::min()) {
        LOG_ERROR << "Invalid value in EventsProcessor::ProcessRelayEvent." << std::endl;
        return;
    }
    if (relayCurrentState._state == relayEvent._state)
        SendCommand(relayEvent._receiver._id, relayEvent._command.dump());
}

void EventsProcessor::ProcessThermostatEvent(const ThermostatEvent& thermostatEvent, const Message& message) {
    auto thermometerCurrentValue = message._data.get<ThermometerCurrentValue>();
    if (thermometerCurrentValue._value == std::numeric_limits<int>::min()) {
        LOG_ERROR << "Invalid value in EventsProcessor::ProcessThermostatEvent." << std::endl;
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

void EventsProcessor::SendCommand(const Uuid& id, const std::string& commandString) {
    try {
        auto storageCache = SimpleTableStorageCache::GetCommandsCache(_queryExecutor);
        SimpleTableInsertOrReplaceInput what;
        what._id = id.data();
        what._data = commandString;
        auto problem = storageCache->InsertOrReplace(what);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems: {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(id);
                if (connection)
                    connection->send_text(commandString);
            } break;
            case StorageCacheProblemType::Empty:
                LOG_ERROR << "Invalid command " << commandString << "." << std::endl;
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
        LOG_ERROR << "EventsProcessor::SendCommand." << std::endl;
    }
}
