#include "DeviceRegistrationProcessor.h"

#include <sstream>

#include "Logger.h"
#include "TimeHelper.h"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    auto& description = message._header._description;
    std::stringstream selectQueryStream;
    selectQueryStream << "SELECT * FROM Devices WHERE id = '"
        << description._id.data()
        << "'";
    selectQueryStream.flush();
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(selectQueryStream.str(), data))
    {
        std::stringstream updateInsertQueryStream;
        if (data.size())
        {
            updateInsertQueryStream
                << "UPDATE Devices SET timestamp = "
                << TimeHelper::TimeToInt(timestamp)
                << " WHERE id = '"
                << description._id.data()
                << "'";
        }
        else
        {
            updateInsertQueryStream
                << "INSERT INTO Devices (id, type, timestamp) VALUES ('"
                << description._id.data()
                << "', '"
                << description._type
                << "', "
                << TimeHelper::TimeToInt(timestamp)
                << ")";
        }
        updateInsertQueryStream.flush();
        if (!_queryExecutor->Execute(updateInsertQueryStream.str()))        
            LOG_SQL_ERROR(updateInsertQueryStream.str());
    }
    else
        LOG_SQL_ERROR(selectQueryStream.str());
    return {};
}