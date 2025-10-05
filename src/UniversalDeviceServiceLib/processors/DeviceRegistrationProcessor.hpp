#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"
#include "DevicesController.hpp"

class DeviceRegistrationProcessor final : public BaseProcessorWithQueryExecutor {
public:
    DeviceRegistrationProcessor(IQueryExecutor* queryExecutor, DevicesController& devicesController);

    virtual ~DeviceRegistrationProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    DevicesController& _devicesController;
};
