#pragma once

#include <string>

#include <crow/http_request.h>
#include <crow/http_response.h>

#include "BaseService.hpp"
#include "DevicesController.hpp"
#include "Middleware.hpp"

class DevicesService final : public BaseService {
public:
    DevicesService(CrowApp& app, DevicesController& devicesController);

    virtual ~DevicesService() = default;

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
