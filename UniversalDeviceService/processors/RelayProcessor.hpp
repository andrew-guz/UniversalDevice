#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"

class RelayProcessor final : public BaseProcessorWithQueryExecutor {
public:
    RelayProcessor(IQueryExecutor* queryExecutor);

    virtual ~RelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};
