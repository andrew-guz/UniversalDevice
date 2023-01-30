#include "ProcessorsFactory.h"

#include "Logger.h"
#include "Constants.h"
#include "DeviceRegistrationProcessor.h"
#include "ThermometerProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message& message, IQueryExecutor* queryExecutor)
{
    static std::vector<std::string> allowedFromTypes = 
    {
        Constants::FrontendServiceType,
        Constants::DeviceTypeThermometer
    };
    static std::vector<std::string> allowedToTypes = 
    {
        Constants::DeviceServiceType,
        Constants::ClientServiceType
    };
    
    auto& messageHeader = message._header;
    auto& from = messageHeader._from;
    auto& to = messageHeader._to;
    if (std::find(allowedFromTypes.begin(), allowedFromTypes.end(), from._type) == allowedFromTypes.end())
    {
        LOG_ERROR << "Invalid from processor type - " << from._type << "." << std::endl;
        return Processors();
    }
    if (std::find(allowedToTypes.begin(), allowedToTypes.end(), to._type) == allowedToTypes.end())
    {
        LOG_ERROR << "Invalid to processor type " << to._type << "." << std::endl;
        return Processors();
    }

    Processors processors;

    if (to._type == Constants::DeviceServiceType)
    {
        if (from._type == Constants::DeviceTypeThermometer)
        {
            processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(queryExecutor)));
            processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(queryExecutor)));            
        }
    }

    return processors;
}