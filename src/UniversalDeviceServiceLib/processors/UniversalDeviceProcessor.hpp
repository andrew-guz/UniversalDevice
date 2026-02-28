#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Message.hpp"
#include "UniversalValue.hpp"
#include "UniversalValuesController.hpp"

class UniversalDeviceProcessor final : public DeviceDataProcessor<UniversalValuesController, UniversalValue, DeviceType::UniversalDevice> {
public:
    UniversalDeviceProcessor(UniversalValuesController& universalValuesController);

    virtual ~UniversalDeviceProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessUniversalDeviceCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
