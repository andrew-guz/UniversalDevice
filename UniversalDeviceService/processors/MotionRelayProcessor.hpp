#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"

class MotionRelayProcessor final : public BaseProcessorWithQueryExecutor {
public:
    MotionRelayProcessor(IQueryExecutor* queryExecutor);

    virtual ~MotionRelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessMotionRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
