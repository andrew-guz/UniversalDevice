#pragma once

#include <string>

#include "BaseService.hpp"

class DeviceWebsocketsService final : public BaseService {
protected:
    DeviceWebsocketsService(IQueryExecutor* queryExecutor);

public:
    virtual ~DeviceWebsocketsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    void OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary);

    void OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason, uint16_t code);

private:
    friend class BaseServiceExtension;
};
