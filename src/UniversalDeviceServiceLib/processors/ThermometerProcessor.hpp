#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Message.hpp"
#include "ThermometerValue.hpp"
#include "ThermometerValuesController.hpp"

class ThermometerProcessor final : public DeviceDataProcessor<ThermometerValuesController, ThermometerValue, DeviceType::Thermometer> {
public:
    ThermometerProcessor(ThermometerValuesController& thermometerValuesController);

    virtual ~ThermometerProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessThermometerCurrentValueMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
