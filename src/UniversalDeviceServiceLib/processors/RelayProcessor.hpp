#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"
#include "RelayValuesController.hpp"

class RelayProcessor final : public BaseProcessorWithQueryExecutor {
public:
    RelayProcessor(IQueryExecutor* queryExecutor, RelayValuesController& relayValuesController);

    virtual ~RelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

private:
    RelayValuesController& _relayValuesController;
};
