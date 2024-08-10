#include "MotionRelayProcessor.hpp"

#include <sstream>

#include "Constants.hpp"
#include "DeviceInformationDescription.hpp"
#include "ExtendedMotionRelayCurrentState.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "MotionRelayCurrentState.hpp"
#include "TimeHelper.hpp"

MotionRelayProcessor::MotionRelayProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json MotionRelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    if (message._header._subject == Constants::SubjectMotionRelayCurrentState) {
        auto currentState = message._data.get<MotionRelayCurrentState>();
        if (currentState._state == std::numeric_limits<float>::min()) {
            LOG_ERROR << "MotionRelayProcessor - invalid message." << std::endl;
            return {};
        }
        auto& description = message._header._description;
        std::stringstream queryStream;
        queryStream << "INSERT INTO MotionRelays (id, timestamp, motion, state) VALUES ('" << description._id.data() << "', "
                    << TimeHelper::TimeToInt(timestamp) << ", '" << currentState._motion << "', '" << currentState._state << "')";
        queryStream.flush();
        if (!_queryExecutor->Execute(queryStream.str())) {
            LOG_SQL_ERROR(queryStream.str());
            return {};
        }
        return Constants::AcknowledgeReply;
    } else if (message._header._subject == Constants::SubjectGetDeviceInformation) {
        auto description = message._data.get<DeviceInformationDescription>();
        if (description._type == Constants::DeviceTypeMotionRelay && !description._id.isEmpty()) {
            std::vector<ExtendedMotionRelayCurrentState> extendedMotionRelayCurrentStates;
            if (description._seconds != 0) {
                auto now = std::chrono::system_clock::now();
                now -= std::chrono::seconds(description._seconds);
                std::stringstream queryStream;
                queryStream << "SELECT timestamp, motion, state FROM MotionRelays WHERE id = '" << description._id.data()
                            << "' AND timestamp >= " << TimeHelper::TimeToInt(now) << " ORDER BY idx DESC";
                queryStream.flush();
                std::vector<std::vector<std::string>> data;
                if (_queryExecutor->Select(queryStream.str(), data))
                    extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
                else
                    LOG_SQL_ERROR(queryStream.str());
            }
            if (extendedMotionRelayCurrentStates.size() == 0) {
                std::stringstream queryStream;
                queryStream << "SELECT timestamp, motion, state FROM MotionRelays WHERE id = '" << description._id.data()
                            << "' ORDER BY idx DESC LIMIT 1";
                queryStream.flush();
                std::vector<std::vector<std::string>> data;
                if (_queryExecutor->Select(queryStream.str(), data))
                    extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
                else
                    LOG_SQL_ERROR(queryStream.str());
            }
            if (extendedMotionRelayCurrentStates.size())
                return extendedMotionRelayCurrentStates;
            else
                LOG_INFO << "No data for device " << description._id.data() << "found." << std::endl;
        }
        return {};
    }

    return {};
}
