#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Message.hpp"
#include "RelayValue.hpp"
#include "RelayValuesController.hpp"

class RelayProcessor final : public DeviceDataProcessor<RelayValuesController, RelayValue, DeviceType::Relay> {
public:
    RelayProcessor(RelayValuesController& relayValuesController);

    virtual ~RelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
