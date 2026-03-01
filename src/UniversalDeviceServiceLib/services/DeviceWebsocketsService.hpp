#pragma once

#include <cstdint>
#include <string>

#include <crow/websocket.h>

#include "BaseService.hpp"
#include "Middleware.hpp"

class DeviceWebsocketsService final : public BaseService {
public:
    DeviceWebsocketsService(CrowApp& app);

    virtual ~DeviceWebsocketsService() = default;

private:
    void OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary);

    void OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason, uint16_t code);

private:
    friend class ServiceExtension;
};
