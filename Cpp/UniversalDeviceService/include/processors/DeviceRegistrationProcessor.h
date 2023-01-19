#ifndef _DEVICE_REGISTRATION_PROCESSOR_H_
#define _DEVICE_REGISTRATION_PROCESSOR_H_

#include "BaseProcessorWithQueryExecutor.h"

class DeviceRegistrationProcessor final : public BaseProcessorWithQueryExecutor
{
public:
    DeviceRegistrationProcessor(IQueryExecutor* queryExecutor);

    virtual ~DeviceRegistrationProcessor() = default;

    virtual void ProcessMessage(const Message& message) override;
};

#endif //_DEVICE_REGISTRATION_PROCESSOR_H_
