#include "MotionRelayProcessor.h"

#include <sstream>

#include "Constants.h"
#include "DeviceInformationDescription.h"
#include "ExtendedMotionRelayCurrentState.h"
#include "JsonExtension.h"
#include "Logger.h"
#include "MotionRelayCurrentState.h"
#include "TimeHelper.h"

MotionRelayProcessor::MotionRelayProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json MotionRelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    if (message._header._subject == Constants::SubjectMotionRelayCurrentState) {
        auto currentState = JsonExtension::CreateFromJson<MotionRelayCurrentState>(message._data);
        if (currentState._state == std::numeric_limits<float>::min()) {
            LOG_ERROR << "MotionRelayProcessor - invalid message." << std::endl;
            return {};
        }
        auto& description = message._header._description;
        std::stringstream queryStream;
        queryStream << "INSERT INTO MotionRelays (id, timestamp, motion, state) VALUES ('" << description._id.data() << "', " << TimeHelper::TimeToInt(timestamp) << ", '" << currentState._motion
                    << "', '" << currentState._state << "')";
        queryStream.flush();
        if (!_queryExecutor->Execute(queryStream.str()))
            LOG_SQL_ERROR(queryStream.str());
        return {};
    } else if (message._header._subject == Constants::SubjectGetDeviceInformation) {
        auto description = JsonExtension::CreateFromJson<DeviceInformationDescription>(message._data);
        if (description._type == Constants::DeviceTypeMotionRelay && !description._id.isEmpty()) {
            std::vector<ExtendedMotionRelayCurrentState> extendedMotionRelayCurrentStates;
            if (description._seconds != 0) {
                auto now = std::chrono::system_clock::now();
                now -= std::chrono::seconds(description._seconds);
                std::stringstream queryStream;
                queryStream << "SELECT timestamp, motion, state FROM MotionRelays WHERE id = '" << description._id.data() << "' AND timestamp >= " << TimeHelper::TimeToInt(now)
                            << " ORDER BY idx DESC";
                queryStream.flush();
                std::vector<std::vector<std::string>> data;
                if (_queryExecutor->Select(queryStream.str(), data))
                    extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
                else
                    LOG_SQL_ERROR(queryStream.str());
            }
            if (extendedMotionRelayCurrentStates.size() == 0) {
                std::stringstream queryStream;
                queryStream << "SELECT timestamp, motion, state FROM MotionRelays WHERE id = '" << description._id.data() << "' ORDER BY idx DESC LIMIT 1";
                queryStream.flush();
                std::vector<std::vector<std::string>> data;
                if (_queryExecutor->Select(queryStream.str(), data))
                    extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
                else
                    LOG_SQL_ERROR(queryStream.str());
            }
            if (extendedMotionRelayCurrentStates.size()) {
                nlohmann::json result;
                for (auto& extendedMotionRelayCurrentState : extendedMotionRelayCurrentStates)
                    result.push_back(extendedMotionRelayCurrentState.ToJson());
                return result;
            } else
                LOG_INFO << "No data for device " << description._id.data() << "found." << std::endl;
        }
        return {};
    }

    return {};
}