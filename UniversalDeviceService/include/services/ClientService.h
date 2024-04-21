#ifndef _CLIENT_SERVICE_H_
#define _CLIENT_SERVICE_H_

#include "BaseService.h"
#include "Event.h"

class ClientService final : public BaseService {
protected:
    ClientService(IQueryExecutor* queryExecutor);

public:
    virtual ~ClientService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response ListDevices();

    crow::response GetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field);

    crow::response SetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field, bool canBeEmpty);

    crow::response GetDeviceInfo(const crow::request& request);

    crow::response GetEvents();

    crow::response AddEvent(const Event& event, const std::string& eventString);

    crow::response UpdateEvent(const Event& event, const std::string& eventString);

    crow::response DeleteEvent(const Event& event);

    crow::response ListLogs();

private:
    friend class BaseServiceExtension;
};

#endif //_CLIENT_SERVICE_H_
