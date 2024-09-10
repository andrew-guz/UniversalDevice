#include "MotionRelayProcessor.hpp"

#include <fmt/format.h>

#include "DeviceInformationDescription.hpp"
#include "ExtendedMotionRelayCurrentState.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "MotionRelayCurrentState.hpp"
#include "TimeHelper.hpp"

MotionRelayProcessor::MotionRelayProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json MotionRelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    switch (message._header._subject) {
        case Subject::MotionRelayCurrentState:
            return ProcessMotionRelayCurrentStateMessage(timestamp, message);
        case Subject::GetDeviceInformation:
            return ProcessGetDeviceInformationMessage(timestamp, message);
        case Subject::Undefined:
        case Subject::TimerEvent:
        case Subject::ThermometerCurrentValue:
        case Subject::RelayCurrentState:
        case Subject::WebSocketAuthorization:
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in MotionRelayProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json MotionRelayProcessor::ProcessMotionRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                           const Message& message) {
    auto currentState = message._data.get<MotionRelayCurrentState>();
    if (currentState._state == std::numeric_limits<float>::min()) {
        LOG_ERROR_MSG("MotionRelayProcessor - invalid message");
        return {};
    }
    auto& description = message._header._description;
    const std::string query = fmt::format("INSERT INTO MotionRelays (id, timestamp, motion, state) VALUES ('{}', {}, '{}', '{}')",
                                          description._id.data(),
                                          TimeHelper::TimeToInt(timestamp),
                                          currentState._motion,
                                          currentState._state);
    if (!_queryExecutor->Execute(query)) {
        LOG_SQL_ERROR(query);
        return {};
    }
    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json MotionRelayProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                        const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::MotionRelay || description._id.isEmpty())
        return {};
    std::vector<ExtendedMotionRelayCurrentState> extendedMotionRelayCurrentStates;
    if (description._seconds != 0) {
        auto now = std::chrono::system_clock::now();
        now -= std::chrono::seconds(description._seconds);
        const std::string query =
            fmt::format("SELECT timestamp, motion, state FROM MotionRelays WHERE id = '{}' AND timestamp >= {} ORDER BY idx DESC",
                        description._id.data(),
                        TimeHelper::TimeToInt(now));
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedMotionRelayCurrentStates.size() == 0) {
        const std::string query =
            fmt::format("SELECT timestamp, motion, state FROM MotionRelays WHERE id = '{}'  ORDER BY idx DESC LIMIT 1", description._id.data());
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedMotionRelayCurrentStates.size())
        return extendedMotionRelayCurrentStates;
    LOG_INFO_MSG(fmt::format("No data for device {} found", description._id.data()));
    return {};
}
