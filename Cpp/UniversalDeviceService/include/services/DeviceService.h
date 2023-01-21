#ifndef _DEVICE_SERVICE_H_
#define _DEVICE_SERVICE_H_

#include "BaseService.h"

class DeviceService final : public BaseService<DeviceService>
{
protected:
    DeviceService(IQueryExecutor* queryExecutor);

public:
    virtual ~DeviceService() = default;

protected:
    virtual void Initialize(crow::SimpleApp& app) override;

    int Inform(const crow::request& request);

private:
    friend class BaseService<DeviceService>;
};

#endif //_DEVICE_SERVICE_H_
