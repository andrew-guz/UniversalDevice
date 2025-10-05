#pragma once

#include "BaseService.hpp"
#include "DevicesController.hpp"

class DevicesService final : public BaseService {
public:
    DevicesService(IQueryExecutor* queryExecutor, DevicesController& devicesController);

    virtual ~DevicesService() = default;

    virtual void Initialize(CrowApp& app) override;

private:
    crow::response ListDevices() const;

    crow::response GetDeviceName(const std::string& idString) const;

    crow::response GetDeviceGroup(const std::string& idString) const;

    crow::response SetDeviceName(const crow::request& request, const std::string& idString);

    crow::response SetDeviceGroup(const crow::request& request, const std::string& idString);

    crow::response GetDeviceInfo(const crow::request& request);

    crow::response DeleteDevice(const std::string& idString);

    crow::response RestartDevice(const std::string& idString);

private:
    DevicesController& _devicesController;
};
