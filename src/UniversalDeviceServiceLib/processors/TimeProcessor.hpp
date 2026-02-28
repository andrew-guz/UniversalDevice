#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "CleanupController.hpp"
#include "IProcessor.hpp"
#include "Message.hpp"

class TimeProcessor final : public IProcessor {
public:
    TimeProcessor(CleanupController& cleanupController);

    virtual ~TimeProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    CleanupController& _cleanupController;
};
