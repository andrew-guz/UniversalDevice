#ifndef _DEVICE_REGISTRATION_PROCESSOR_H_
#define _DEVICE_REGISTRATION_PROCESSOR_H_

#include "BaseProcessorWithQueryExecutor.h"

class DeviceRegistrationProcessor final : public BaseProcessorWithQueryExecutor {
public:
    DeviceRegistrationProcessor(IQueryExecutor* queryExecutor);

    virtual ~DeviceRegistrationProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};

#endif //_DEVICE_REGISTRATION_PROCESSOR_H_
