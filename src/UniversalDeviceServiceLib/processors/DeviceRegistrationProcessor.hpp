#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "DevicesController.hpp"
#include "IProcessor.hpp"
#include "Message.hpp"

class DeviceRegistrationProcessor final : public IProcessor {
public:
    DeviceRegistrationProcessor(DevicesController& devicesController);

    virtual ~DeviceRegistrationProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    DevicesController& _devicesController;
};
