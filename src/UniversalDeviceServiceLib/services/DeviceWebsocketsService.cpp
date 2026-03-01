#include "DeviceWebsocketsService.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include <crow/app.h>
#include <crow/websocket.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "AccountManager.hpp"
#include "BaseService.hpp"
#include "Defines.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Middleware.hpp"
#include "Provider.hpp"
#include "WebSocketAuthentication.hpp"
#include "WebsocketsCache.hpp"

DeviceWebsocketsService::DeviceWebsocketsService(CrowApp& app) {
    CROW_WEBSOCKET_ROUTE(app, API_DEVICE_WEBSOCKETS)
        .onopen([&](crow::websocket::connection& connection) { LOG_INFO_MSG(fmt::format("Incoming ip - {}", connection.get_remote_ip())); })
        .onmessage([&](crow::websocket::connection& connection, const std::string& data, bool is_binary) {
            return OnWebSocketMessage(connection, data, is_binary);
        })
        .onclose([&](crow::websocket::connection& connection, const std::string& reason, const uint16_t code) {
            return OnWebSocketClose(connection, reason, code);
        });
}

void DeviceWebsocketsService::OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary) {
    if (is_binary)
        return;
    try {
        auto timestamp = std::chrono::system_clock::now();
        auto message = ServiceExtension::GetMessageFromWebSocketData(data);
        if (message._header._subject == Subject::WebSocketAuthorization) {
            auto webSocketAuthentication = message._data.get<WebSocketAuthentication>();
            if (AccountManager::Instance()->IsValidUser(webSocketAuthentication._authString))
                WebsocketsCache::Instance()->AddWebSocketConnection(GetProviderId(message._header._description), connection);
            else {
                LOG_ERROR_MSG("Not authorized connection");
                // ask for authorization again
                connection.send_text(nlohmann::json{
                    { "reauthorize", true },
                }
                                         .dump());
            }
        } else {
            auto knownConnection = WebsocketsCache::Instance()->GetWebSocketConnection(GetProviderId(message._header._description));
            if (knownConnection) {
                // not all messages need answers
                auto result = CallProcessorsJsonResult(timestamp, message);
                if (!result.is_null())
                    connection.send_text(result.dump());
            } else {
                LOG_ERROR_MSG("Not authorized connection");
                // ask for authorization again
                connection.send_text(nlohmann::json{
                    { "reauthorize", true },
                }
                                         .dump());
            }
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::OnWebSocketMessage");
    }
}

void DeviceWebsocketsService::OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason, const uint16_t code) {
    WebsocketsCache::Instance()->DeleteWebSocketConnection(connection);
}
