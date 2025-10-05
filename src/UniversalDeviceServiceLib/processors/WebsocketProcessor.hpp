#pragma once

#include "BaseProcessorWithQueryExecutor.hpp"
#include "CommandsController.hpp"
#include "SettingsController.hpp"

class WebSocketProcessor final : public BaseProcessorWithQueryExecutor {
public:
    WebSocketProcessor(IQueryExecutor* queryExecutor, SettingsController& settingsController, CommandsController& commandsController);

    virtual ~WebSocketProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessWebSocketGetSettingsMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessWebSocketGetCommandsMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

private:
    SettingsController& _settingsController;
    CommandsController& _commandsController;
};
