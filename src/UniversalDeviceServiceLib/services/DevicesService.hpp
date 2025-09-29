#pragma once

#include "BaseService.hpp"
#include "SettingsController.hpp"

class DevicesService final : public BaseService {
public:
    DevicesService(IQueryExecutor* queryExecutor, SettingsController& settingsController);

    virtual ~DevicesService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response ListDevices() const;

    crow::response GetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field) const;

    crow::response GetDeviceName(const crow::request& request, const std::string& idString) const;

    crow::response GetDeviceGroup(const crow::request& request, const std::string& idString) const;

    crow::response SetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field, bool canBeEmpty);

    crow::response SetDeviceName(const crow::request& request, const std::string& idString);

    crow::response SetDeviceGroup(const crow::request& request, const std::string& idString);

    crow::response GetDeviceInfo(const crow::request& request);

    crow::response DeleteDevice(const std::string& idString);

    crow::response RestartDevice(const std::string& idString);

private:
    SettingsController& _settingsController;
};
