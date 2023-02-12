#include "RelayProcessor.h"

#include <sstream>

#include "Logger.h"
#include "Constants.h"
#include "RelayCurrentState.h"
#include "ExtendedRelayCurrentState.h"
#include "TimeHelper.h"

RelayProcessor::RelayProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json RelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    if (message._header._subject == Constants::SubjectRelayCurrentState)
    {
        auto currentState = JsonExtension::CreateFromJson<RelayCurrentState>(message._data);
        if (currentState._state == std::numeric_limits<float>::min())
        {
            LOG_ERROR << "RelayProcessor - invalid message." << std::endl;
            return {};
        }
        auto& description = message._header._description;
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO Relays (id, timestamp, state) VALUES ('"
            << description._id.data()
            << "', '" 
            << TimeHelper::TimeToString(timestamp)
            << "', '" 
            << currentState._state
            << "')";
        queryStream.flush();
        if (!_queryExecutor->Execute(queryStream.str()))
            LOG_SQL_ERROR(queryStream.str());
        return {};
    }
    else if (message._header._subject == Constants::SubjectGetDeviceInformation)
    {
        auto description = JsonExtension::CreateFromJson<ComponentDescription>(message._data);
        if (description._type == Constants::DeviceTypeRelay &&
            !description._id.isEmpty())
        {
            std::stringstream queryStream;
            queryStream
                << "SELECT timestamp, state FROM Relays WHERE id = '"
                << description._id.data()
                << "' ORDER BY idx DESC LIMIT 100";
            queryStream.flush();
            nlohmann::json result;
            std::vector<std::vector<std::string>> data;
            if (_queryExecutor->Select(queryStream.str(), data))
            {
                auto extendedRelayCurrentStates = DbExtension::CreateVectorFromDbStrings<ExtendedRelayCurrentState>(data);
                for (auto& extendedRelayCurrentState : extendedRelayCurrentStates)
                    result.push_back(extendedRelayCurrentState.ToJson());
                return result;
            }
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        return {};
    }
    
    return {};
}