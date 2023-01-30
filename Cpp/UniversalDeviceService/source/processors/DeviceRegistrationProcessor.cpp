#include "DeviceRegistrationProcessor.h"

#include <sstream>

#include "Logger.h"
#include "TimeHelper.h"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

void DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    auto& from = message._header._from;
    std::stringstream queryStream;
    queryStream
        << "INSERT OR REPLACE INTO Devices (id, type, timestamp) VALUES ('"
        << from._id.data()
        << "', '" 
        << from._type 
        << "', '" 
        << TimeHelper::TimeToString(timestamp)
        << "')";
    queryStream.flush();
    _queryExecutor->Execute(queryStream.str());
}