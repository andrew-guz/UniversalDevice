#ifndef _DEVICE_REGISTRATION_PROCESSOR_H_
#define _DEVICE_REGISTRATION_PROCESSOR_H_

#include "BaseProcessorUsingStorage.h"

class DeviceRegistrationProcessor final : public BaseProcessorUsingStorage
{
public:
    DeviceRegistrationProcessor(Storage& storage);

    virtual ~DeviceRegistrationProcessor() = default;

    virtual void ProcessMessage(const Message& message) override;
};

#endif //_DEVICE_REGISTRATION_PROCESSOR_H_
