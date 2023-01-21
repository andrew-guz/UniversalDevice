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

    std::string ListDevices();

private:
    friend class BaseService<ClientService>;
};

#endif //_CLIENT_SERVICE_H_
