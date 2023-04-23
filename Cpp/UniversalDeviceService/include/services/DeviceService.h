#ifndef _DEVICE_SERVICE_H_
#define _DEVICE_SERVICE_H_

#include <mutex>
#include <map>
#include <string>

#include "BaseService.h"

class DeviceService final : public BaseService
{
protected:
    DeviceService(IQueryExecutor* queryExecutor);

public:
    virtual ~DeviceService() = default;

protected:
    virtual void Initialize(crow::SimpleApp& app) override;

    crow::response GetSettings(const crow::request& request, const std::string& idString);

    crow::response SetSettings(const crow::request& request, const std::string& idString);

    crow::response GetCommands(const crow::request& request, const std::string& idString);

    crow::response SetCommands(const crow::request& request, const std::string& idString);

    crow::response Inform(const crow::request& request);

    void AddWebSocketConnection(const Uuid& id, crow::websocket::connection& connection);

    crow::websocket::connection* GetWebSocketConnection(const Uuid& id);

    void DeleteWebSocketConnection(crow::websocket::connection& connection);

    void OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary);

    void OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason);

    void TimerFunction();

private:
    std::mutex                                      _webSocketConnectionsMutex;
    std::map<Uuid, crow::websocket::connection*>    _webSocketConnections;

private:
    friend class BaseServiceExtension;
};

#endif //_DEVICE_SERVICE_H_
