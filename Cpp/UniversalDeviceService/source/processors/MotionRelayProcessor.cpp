#include "MotionRelayProcessor.h"

#include <sstream>

#include "Logger.h"
#include "Constants.h"
#include "JsonExtension.h"
#include "MotionRelayCurrentState.h"
#include "ExtendedMotionRelayCurrentState.h"
#include "TimeHelper.h"

MotionRelayProcessor::MotionRelayProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json MotionRelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    if (message._header._subject == Constants::SubjectMotionRelayCurrentState)
    {
        auto currentState = JsonExtension::CreateFromJson<MotionRelayCurrentState>(message._data);
        if (currentState._state == std::numeric_limits<float>::min())
        {
            LOG_ERROR << "MotionRelayProcessor - invalid message." << std::endl;
            return {};
        }
        auto& description = message._header._description;
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO MotionRelays (id, timestamp, motion, state) VALUES ('"
            << description._id.data()
            << "', '" 
            << TimeHelper::TimeToString(timestamp)
            << "', '" 
            << currentState._motion
            << "', '" 
            << currentState._state
            << "')";
        queryStream.flush();
        if (!_queryExecutor->Execute(queryStream.str()))
            LOG_SQL_ERROR(queryStream.str());
        return {};
    }
    else if (message._header._subject == Constants::SubjectGetDeviceInformationSingle ||
             message._header._subject == Constants::SubjectGetDeviceInformationMultiple)
    {
        auto description = JsonExtension::CreateFromJson<ComponentDescription>(message._data);
        if (description._type == Constants::DeviceTypeMotionRelay &&
            !description._id.isEmpty())
        {
            std::stringstream queryStream;
            queryStream
                << "SELECT timestamp, motion, state FROM MotionRelays WHERE id = '"
                << description._id.data()
                << "' ORDER BY idx DESC LIMIT "
                << (message._header._subject == Constants::SubjectGetDeviceInformationSingle ? "1" : "120");
            queryStream.flush();
            nlohmann::json result;
            std::vector<std::vector<std::string>> data;
            if (_queryExecutor->Select(queryStream.str(), data))
            {
                auto extendedMotionRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedMotionRelayCurrentState>(data);
                for (auto& extendedMotionRelayCurrentState : extendedMotionRelayCurrentStates)
                    result.push_back(extendedMotionRelayCurrentState.ToJson());
                return result;
            }
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        return {};
    }
    
    return {};
}