#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"

class UniversalDeviceProcessor final : public BaseProcessorWithQueryExecutor {
public:
    UniversalDeviceProcessor(IQueryExecutor* queryExecutor);

    virtual ~UniversalDeviceProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessUniversalDeviceCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
