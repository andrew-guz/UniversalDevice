#ifndef _CLIENT_SERVICE_H_
#define _CLIENT_SERVICE_H_

#include "BaseService.h"

class ClientService final : public BaseService<ClientService>
{
protected:
    ClientService(IQueryExecutor* queryExecutor);

public:
    virtual ~ClientService() = default;

protected:
    virtual void Initialize(crow::SimpleApp& app) override;

    std::string ListDevices(const crow::request& request);

    std::string GetDeviceInfo(const crow::request& request);

private:
    friend class BaseService<ClientService>;
};

#endif //_CLIENT_SERVICE_H_
