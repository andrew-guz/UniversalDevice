#include "ProcessorsFactory.h"

#include "Logger.h"
#include "Constants.h"
#include "DeviceRegistrationProcessor.h"
#include "ThermometerProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor)
{
    auto& messageHeader = message._header;

    Processors processors;

    if (messageHeader._subject == Constants::SubjectGetDeviceInformation)
    {
        //we need information about device - here we should call all device processors maybe some one will return data
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));
    }
    else if (messageHeader._subject == Constants::SubjectThermometerCurrentValue) //concrete message to register ned data from thermometer
    {
        //register thermometer if needed
        processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
        //since this is thermometer - add ThermometerProcessor
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));            
    }

    return processors;
}