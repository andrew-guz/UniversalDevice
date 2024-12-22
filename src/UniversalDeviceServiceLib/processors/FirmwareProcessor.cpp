#include "FirmwareProcessor.hpp"

#include <filesystem>

#include <fmt/format.h>

#include "Logger.hpp"
#include "Settings.hpp"
#include "Uuid.hpp"
#include "WebsocketsCache.hpp"

FirmwareProcessor::FirmwareProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json FirmwareProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    try {
        switch (message._header._subject) {
            case Subject::WebSocketGetSettings:
            case Subject::WebSocketGetCommands:
            case Subject::Undefined:
            case Subject::TimerEvent:
                UpdateFirmwareIfNeeded();
            case Subject::GetDeviceInformation:
            case Subject::ThermometerCurrentValue:
            case Subject::RelayCurrentState:
            case Subject::MotionRelayCurrentState:
            case Subject::WebSocketAuthorization:
                return {};
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in FirmwareProcessor::ProcessMessage");
        return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in FirmwareProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

void FirmwareProcessor::UpdateFirmwareIfNeeded() {
    const Settings settings = Settings::ReadSettings();

    if (settings._firmwarePath.empty()) {
        LOG_INFO_MSG("No firmware path provided");
        return;
    }

    if (!std::filesystem::exists(settings._firmwarePath))
        return;

    for (const auto& entry : std::filesystem::directory_iterator(settings._firmwarePath)) {
        if (entry.is_directory()) {
            const Uuid id{ entry.path().filename().string() };
            if (id.isEmpty())
                continue;

            crow::websocket::connection* connection = WebsocketsCache::Instance()->GetWebSocketConnection(id);
            if (connection)
                connection->send_text("{ \"update_firmware\" : true }");
        }
    }
}
