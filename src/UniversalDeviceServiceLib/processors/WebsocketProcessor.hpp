#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"

class WebSocketProcessor final : public BaseProcessorWithQueryExecutor {
public:
    WebSocketProcessor(IQueryExecutor* queryExecutor);

    virtual ~WebSocketProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessWebSocketGetSettingsMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessWebSocketGetCommandsMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};
