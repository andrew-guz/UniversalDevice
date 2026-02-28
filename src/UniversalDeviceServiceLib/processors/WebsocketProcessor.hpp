#pragma once

#include <chrono>

#include <nlohmann/json_fwd.hpp>

#include "CommandsController.hpp"
#include "IProcessor.hpp"
#include "Message.hpp"
#include "SettingsController.hpp"

class WebSocketProcessor final : public IProcessor {
public:
    WebSocketProcessor(SettingsController& settingsController, CommandsController& commandsController);

    virtual ~WebSocketProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    nlohmann::json ProcessWebSocketGetSettingsMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json ProcessWebSocketGetCommandsMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message);

private:
    SettingsController& _settingsController;
    CommandsController& _commandsController;
};
