#ifndef _CLIENT_SERVICE_H_
#define _CLIENT_SERVICE_H_

#include "BaseService.h"

class ClientService final : public BaseService
{
protected:
    ClientService(IQueryExecutor* queryExecutor);

public:
    virtual ~ClientService() = default;

protected:
    virtual void Initialize(crow::SimpleApp& app) override;

    crow::response ListDevices(const crow::request& request);

    crow::response GetDeviceName(const crow::request& request, const std::string& idString);

    crow::response SetDeviceName(const crow::request& request, const std::string& idString);

    crow::response GetDeviceInfo(const crow::request& request);

    crow::response GetEvents(const crow::request& request);

    crow::response AddEvent(const crow::request& request, const std::string& idString);

    crow::response UpdateEvent(const crow::request& request, const std::string& idString);

    crow::response DeleteEvent(const crow::request& request, const std::string& idString);

private:
    friend class BaseServiceExtension;
};

#endif //_CLIENT_SERVICE_H_
