#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Message.hpp"
#include "MotionRelayValue.hpp"
#include "MotionRelayValuesController.hpp"

class MotionRelayProcessor final : public DeviceDataProcessor<MotionRelayValuesController, MotionRelayValue, DeviceType::MotionRelay> {
public:
    MotionRelayProcessor(MotionRelayValuesController& motionRelayValuesController);

    virtual ~MotionRelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessMotionRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
