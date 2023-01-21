#include "DeviceRegistrationProcessor.h"

#include <sstream>

#include "TimeHelper.h"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

void DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    auto& deviceDescription = message._header._deviceDescription;
    std::stringstream queryStream;
    queryStream
        << "INSERT OR REPLACE INTO 'Devices' ('id', 'type', 'timestamp') VALUES ('"
        << deviceDescription._id.data()
        << "', '" 
        << deviceDescription._type 
        << "', '" 
        << TimeHelper::TimeToString(timestamp)
        << "')";
    queryStream.flush();
    _queryExecutor->Execute(queryStream.str());
}