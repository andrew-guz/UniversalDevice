#include "RelayProcessor.hpp"

#include <sstream>

#include "Constants.hpp"
#include "DeviceInformationDescription.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayCurrentState.hpp"
#include "TimeHelper.hpp"

RelayProcessor::RelayProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

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
    LOG_ERROR << "Unknown subject to process in RelayProcessor: " << static_cast<int>(message._header._subject) << std::endl;
    return {};
}

nlohmann::json RelayProcessor::ProcessRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    auto currentState = message._data.get<RelayCurrentState>();
    if (currentState._state == std::numeric_limits<float>::min()) {
        LOG_ERROR << "RelayProcessor - invalid message." << std::endl;
        return {};
    }
    auto& description = message._header._description;
    std::stringstream queryStream;
    queryStream << "INSERT INTO Relays (id, timestamp, state) VALUES ('" << description._id.data() << "', " << TimeHelper::TimeToInt(timestamp)
                << ", '" << currentState._state << "')";
    queryStream.flush();
    if (!_queryExecutor->Execute(queryStream.str())) {
        LOG_SQL_ERROR(queryStream.str());
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
        std::stringstream queryStream;
        queryStream << "SELECT timestamp, state FROM Relays WHERE id = '" << description._id.data()
                    << "' AND timestamp >= " << TimeHelper::TimeToInt(now) << " ORDER BY idx DESC";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
            extendedRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedRelayCurrentState>(data);
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    if (extendedRelayCurrentStates.size() == 0) {
        std::stringstream queryStream;
        queryStream << "SELECT timestamp, state FROM Relays WHERE id = '" << description._id.data() << "' ORDER BY idx DESC LIMIT 1";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
            extendedRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedRelayCurrentState>(data);
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    if (extendedRelayCurrentStates.size())
        return extendedRelayCurrentStates;
    LOG_INFO << "No data for device " << description._id.data() << "found." << std::endl;
    return {};
}
