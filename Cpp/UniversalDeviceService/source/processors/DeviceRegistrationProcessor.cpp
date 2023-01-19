#include "DeviceRegistrationProcessor.h"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

void DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message &message)
{
    //TODO
}