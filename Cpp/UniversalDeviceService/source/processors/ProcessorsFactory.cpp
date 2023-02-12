#include "ProcessorsFactory.h"

#include "Logger.h"
#include "Constants.h"
#include "DeviceRegistrationProcessor.h"
#include "ThermometerProcessor.h"
#include "RelayProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor)
{
    auto& messageHeader = message._header;

    Processors processors;

    if (messageHeader._subject == Constants::SubjectGetDeviceInformation)
    {
        //we need information about device - here we should call all device processors maybe some one will return data
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));
        processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor)));
    }
    else if (messageHeader._subject == Constants::SubjectThermometerCurrentValue) //concrete message to register new data from thermometer
    {
        //register thermometer if needed
        processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
        //since this is thermometer - add ThermometerProcessor
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));            
    }
    else if (messageHeader._subject == Constants::SubjectRelayCurrentState) //concrete message to register new data from relay
    {
        //register relay if needed
        processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
        //since this is relay - add RelayProcessor
        processors.push_back(std::shared_ptr<IProcessor>(new RelayProcessor(queryExecutor)));
    }

    return processors;
}