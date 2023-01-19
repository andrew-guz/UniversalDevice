#include "ProcessorsFactory.h"

#include "DeviceRegistrationProcessor.h"

Processors ProcessorsFactory::CreateProcessors(const Message&message, Storage& storage)
{
    Processors processors;
    //always add DeviceRegistrationProcessor
    processors.push_back(std::shared_ptr<IProcessor>(new DeviceRegistrationProcessor(storage)));
    return processors;
}