#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"

class FirmwareProcessor final : public BaseProcessorWithQueryExecutor {
public:
    FirmwareProcessor(IQueryExecutor* queryExecutor);

    virtual ~FirmwareProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    void UpdateFirmwareIfNeeded();
};
