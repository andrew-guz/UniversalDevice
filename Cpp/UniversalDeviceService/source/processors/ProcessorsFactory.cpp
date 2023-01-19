#include "ProcessorsFactory.h"

#include <iostream>

#include "Constants.h"
#include "DeviceRegistrationProcessor.h"
#include "ThermometerProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message& message, Storage& storage)
{
    static std::vector<std::string> allowedTypes = 
    {
        Constants::DeviceTypeThermometer
    };
    
    if (std::find(allowedTypes.begin(), allowedTypes.end(), message._header._type) == allowedTypes.end())
    {
        std::cout << "No processors found for type " << message._header._type << std::endl;
        return Processors();
    }

    Processors processors;

    //always add DeviceRegistrationProcessor
    processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(storage)));

    if (message._header._subject.compare(Constants::DeviceTypeThermometer) == 0)
        processors.push_back(std::shared_ptr<IProcessor>(new ThermometerProcessor(storage)));

    return processors;
}