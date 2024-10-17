#include "RelayProcessor.hpp"

#include <fmt/format.h>

#include "DeviceInformationDescription.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayCurrentState.hpp"
#include "TimeHelper.hpp"

RelayProcessor::RelayProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json RelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    switch (message._header._subject) {
        case Subject::RelayCurrentState:
            return ProcessRelayCurrentStateMessage(timestamp, message);
        case Subject::GetDeviceInformation:
            return ProcessGetDeviceInformationMessage(timestamp, message);
        case Subject::Undefined:
        case Subject::TimerEvent:
        case Subject::ThermometerCurrentValue:
        case Subject::MotionRelayCurrentState:
        case Subject::WebSocketAuthorization:
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in RelayProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json RelayProcessor::ProcessRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    auto currentState = message._data.get<RelayCurrentState>();
    if (currentState._state == std::numeric_limits<float>::min()) {
        LOG_ERROR_MSG("RelayProcessor - invalid message");
        return {};
    }
    auto& description = message._header._description;
    const std::string query = fmt::format("INSERT INTO Relays (id, timestamp, state) VALUES ('{}', {}, '{}')",
                                          description._id.data(),
                                          TimeHelper::TimeToInt(timestamp),
                                          currentState._state);
    if (!_queryExecutor->Execute(query)) {
        LOG_SQL_ERROR(query);
        return {};
    }
    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json RelayProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::Relay || description._id.isEmpty())
        return {};
    std::vector<ExtendedRelayCurrentState> extendedRelayCurrentStates;
    if (description._seconds != 0) {
        auto now = std::chrono::system_clock::now();
        now -= std::chrono::seconds(description._seconds);
        const std::string query = fmt::format("SELECT timestamp, state FROM Relays WHERE id = '{}' AND timestamp >= {} ORDER BY idx DESC",
                                              description._id.data(),
                                              TimeHelper::TimeToInt(now));
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedRelayCurrentState>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedRelayCurrentStates.size() == 0) {
        const std::string query =
            fmt::format("SELECT timestamp, state FROM Relays WHERE id = '{}' ORDER BY idx DESC LIMIT 1", description._id.data());
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedRelayCurrentState>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedRelayCurrentStates.size())
        return extendedRelayCurrentStates;
    LOG_INFO_MSG(fmt::format("No data for device {} found", description._id.data()));
    return {};
}