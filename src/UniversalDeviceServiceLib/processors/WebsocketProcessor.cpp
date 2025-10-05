#include "WebsocketProcessor.hpp"

#include <optional>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "Command.hpp"
#include "Marshaling.hpp"

WebSocketProcessor::WebSocketProcessor(IQueryExecutor* queryExecutor,
                                       SettingsController& settingsController,
                                       CommandsController& commandsController) :
    BaseProcessorWithQueryExecutor(queryExecutor),
    _settingsController(settingsController),
    _commandsController(commandsController) {}

nlohmann::json WebSocketProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    try {
        switch (message._header._subject) {
            case Subject::WebSocketGetSettings:
                return ProcessWebSocketGetSettingsMessage(timestamp, message);
            case Subject::WebSocketGetCommands:
                return ProcessWebSocketGetCommandsMessage(timestamp, message);
            case Subject::Undefined:
            case Subject::TimerEvent:
            case Subject::GetDeviceInformation:
            case Subject::ThermometerCurrentValue:
            case Subject::RelayCurrentState:
            case Subject::MotionRelayCurrentState:
            case Subject::WebSocketAuthorization:
            case Subject::UniversalDeviceCurrentState:
            case Subject::SunriseEvent:
            case Subject::SunsetEvent:
                return {};
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in WebSocketProcessor::ProcessMessage");
        return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in WebSocketProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json WebSocketProcessor::ProcessWebSocketGetSettingsMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                      const Message& message) {
    const std::optional<Settings> settings = _settingsController.Get(message._header._description._id);
    if (settings.has_value())
        return static_cast<nlohmann::json>(settings.value());

    return {};
}

nlohmann::json WebSocketProcessor::ProcessWebSocketGetCommandsMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                      const Message& message) {
    const std::optional<Command> command = _commandsController.Get(message._header._description._id);
    if (command.has_value())
        return static_cast<nlohmann::json>(command.value());

    return {};
}
