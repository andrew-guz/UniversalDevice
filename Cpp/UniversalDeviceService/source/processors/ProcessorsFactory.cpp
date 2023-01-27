#include "ProcessorsFactory.h"

#include "Logger.h"
#include "Constants.h"
#include "DeviceRegistrationProcessor.h"
#include "ThermometerProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor)
{
    static std::vector<std::string> allowedTypes = 
    {
        Constants::DeviceTypeThermometer
    };
    
    auto& messageHeader = message._header;
    auto& deviceDescription = messageHeader._deviceDescription;
    if (std::find(allowedTypes.begin(), allowedTypes.end(), deviceDescription._type) == allowedTypes.end())
    {
        LOG_ERROR << "No processors found for type " << deviceDescription._type << "." << std::endl;
        return Processors();
    }

    Processors processors;

    //always add DeviceRegistrationProcessor
    processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));

    if (deviceDescription._type == Constants::DeviceTypeThermometer)
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));

    return processors;
}