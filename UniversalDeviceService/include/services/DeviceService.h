#ifndef _DEVICE_SERVICE_H_
#define _DEVICE_SERVICE_H_

#include <string>

#include "BaseService.h"

class DeviceService final : public BaseService {
protected:
    DeviceService(IQueryExecutor* queryExecutor);

public:
    virtual ~DeviceService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

    crow::response GetSettings(const std::string& idString);

    crow::response SetSettings(const crow::request& request, const std::string& idString);

    crow::response GetCommands(const std::string& idString);

    crow::response SetCommands(const crow::request& request, const std::string& idString);

    crow::response DeleteDevice(const std::string& idString);

    void OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary);

    void OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason);

    void TimerFunction();

private:
    friend class BaseServiceExtension;
};

#endif //_DEVICE_SERVICE_H_
