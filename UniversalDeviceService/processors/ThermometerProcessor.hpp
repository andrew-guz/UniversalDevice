#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"

class ThermometerProcessor final : public BaseProcessorWithQueryExecutor {
public:
    ThermometerProcessor(IQueryExecutor* queryExecutor);

    virtual ~ThermometerProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};
