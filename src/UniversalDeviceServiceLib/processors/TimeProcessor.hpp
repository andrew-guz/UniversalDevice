#pragma once

#include <vector>

#include "BaseProcessorWithQueryExecutor.hpp"

class TimeProcessor final : public BaseProcessorWithQueryExecutor {
public:
    TimeProcessor(IQueryExecutor* queryExecutor);

    virtual ~TimeProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};
