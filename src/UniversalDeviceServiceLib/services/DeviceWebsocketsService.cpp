#include "DeviceWebsocketsService.hpp"

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "AccountManager.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "WebSocketAuthentication.hpp"
#include "WebsocketsCache.hpp"

DeviceWebsocketsService::DeviceWebsocketsService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void DeviceWebsocketsService::Initialize(CrowApp& app) {

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
        auto message = BaseServiceExtension::GetMessageFromWebSocketData(data);
        if (message._header._subject == Subject::WebSocketAuthorization) {
            auto webSocketAuthentication = message._data.get<WebSocketAuthentication>();
            if (AccountManager::Instance()->IsValidUser(webSocketAuthentication._authString))
                WebsocketsCache::Instance()->AddWebSocketConnection(message._header._description._id, connection);
            else {
                LOG_ERROR_MSG("Not authorized connection");
                // ask for authorization again
                connection.send_text(nlohmann::json{
                    { "reauthorize", true },
                }
                                         .dump());
            }
        } else {
            auto knownConnection = WebsocketsCache::Instance()->GetWebSocketConnection(message._header._description._id);
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
