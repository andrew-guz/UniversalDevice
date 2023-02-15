#ifndef _DEVICE_SERVICE_H_
#define _DEVICE_SERVICE_H_

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

    void TimerFunction();

private:
    friend class BaseServiceExtension;
};

#endif //_DEVICE_SERVICE_H_
