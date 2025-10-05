#include "DevicesService.hpp"

#include <optional>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "BaseService.hpp"
#include "Defines.hpp"
#include "DeviceProperty.hpp"
#include "Logger.hpp"
#include "Uuid.hpp"
#include "WebsocketsCache.hpp"

DevicesService::DevicesService(IQueryExecutor* queryExecutor, DevicesController& devicesController) :
    BaseService(queryExecutor),
    _devicesController(devicesController) {}

void DevicesService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_DEVICES).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &DevicesService::ListDevices));
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &DevicesService::GetDeviceName));
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::POST)(ServiceExtension::bind(this, &DevicesService::SetDeviceName));
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &DevicesService::GetDeviceGroup));
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::POST)(ServiceExtension::bind(this, &DevicesService::SetDeviceGroup));
    CROW_ROUTE(app, API_CLIENT_DEVICE_GET_INFO).methods(crow::HTTPMethod::POST)(ServiceExtension::bind(this, &DevicesService::GetDeviceInfo));
    CROW_ROUTE(app, API_DEVICE).methods(crow::HTTPMethod::DELETE)(ServiceExtension::bind(this, &DevicesService::DeleteDevice));
    CROW_ROUTE(app, API_CLIENT_RESTART_DEVICE).methods(crow::HTTPMethod::POST)(ServiceExtension::bind(this, &DevicesService::RestartDevice));
}

crow::response DevicesService::ListDevices() const {
    nlohmann::json result;
    try {
        result = _devicesController.List();
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::ListDevices");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, result.dump());
}

crow::response DevicesService::GetDeviceName(const std::string& idString) const {
    std::optional<Device> device = _devicesController.Get(Uuid{ idString });
    if (device.has_value()) {
        const nlohmann::json result = DeviceProperty{ device->_name };
        return crow::response(crow::OK, result.dump());
    }

    LOG_ERROR_MSG("Something went wrong in ClientService::GetDeviceName");

    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::GetDeviceGroup(const std::string& idString) const {
    std::optional<Device> device = _devicesController.Get(Uuid{ idString });
    if (device.has_value()) {
        const nlohmann::json result = DeviceProperty{ device->_group };
        return crow::response(crow::OK, result.dump());
    }

    LOG_ERROR_MSG("Something went wrong in ClientService::GetDeviceGroup");

    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::SetDeviceName(const crow::request& request, const std::string& idString) {
    try {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto deviceProperty = bodyJson.get<DeviceProperty>();
        if (deviceProperty._value.size()) {
            if (_devicesController.UpdateDeviceName(Uuid{ idString }, deviceProperty._value))
                return crow::response(crow::OK);
            else
                LOG_ERROR_MSG(fmt::format("Failed to update device {} name {}", idString, request.body));
        } else
            LOG_ERROR_MSG(fmt::format("Invalid device name {}", request.body));
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::SetDeviceProperty");
    }

    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::SetDeviceGroup(const crow::request& request, const std::string& idString) {
    try {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto deviceProperty = bodyJson.get<DeviceProperty>();
        if (_devicesController.UpdateDeviceGroup(Uuid{ idString }, deviceProperty._value))
            return crow::response(crow::OK);
        else
            LOG_ERROR_MSG(fmt::format("Failed to update device {} group {}", idString, request.body));
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::SetDeviceGroup");
    }

    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::GetDeviceInfo(const crow::request& request) {
    nlohmann::json result;

    try {
        auto timestamp = std::chrono::system_clock::now();
        auto message = ServiceExtension::GetMessageFromRequest(request);
        result = CallProcessorsJsonResult(timestamp, message);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::GetDeviceInfo");
    }

    return crow::response(crow::OK, result.dump());
}

crow::response DevicesService::DeleteDevice(const std::string& idString) {
    try {
        if (_devicesController.Remove(Uuid{ idString })) {
            // TODO: Clean event table. Right now deleted id is not a column of event table...
            return crow::response{ crow::OK };
        }

        LOG_ERROR_MSG(fmt::format("Failed to delete device {}", idString));

        return crow::response(crow::BAD_REQUEST);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::DeleteDevice");
    }

    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::RestartDevice(const std::string& idString) {
    auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid{ idString });
    if (connection) {
        LOG_INFO_MSG(fmt::format("Restarting device {}", idString));
        connection->send_text(nlohmann::json{
            { "restart", true },
        }
                                  .dump());
        return crow::response(crow::OK);
    }

    LOG_ERROR_MSG(fmt::format("Failed to restart device {} - not connected", idString));

    return crow::response(crow::BAD_REQUEST);
}
