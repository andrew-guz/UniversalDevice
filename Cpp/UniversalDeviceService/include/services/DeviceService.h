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

    std::string GetSettings(const std::string& idString);

    int SetSettings(const std::string& idString, const crow::request& request);

    int Inform(const crow::request& request);

private:
    friend class BaseServiceExtension;
};

#endif //_DEVICE_SERVICE_H_
