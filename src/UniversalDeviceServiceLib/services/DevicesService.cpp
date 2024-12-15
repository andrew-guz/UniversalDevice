#include "DevicesService.hpp"

#include "fmt/format.h"

#include "DbExtension.hpp"
#include "Defines.hpp"
#include "DeviceProperty.hpp"
#include "ExtendedComponentDescription.hpp"
#include "Logger.hpp"
#include "SimpleTableStorageCache.hpp"
#include "Uuid.hpp"
#include "WebsocketsCache.hpp"

DevicesService::DevicesService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void DevicesService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_DEVICES).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &DevicesService::ListDevices));
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &DevicesService::GetDeviceName));
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &DevicesService::SetDeviceName));
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &DevicesService::GetDeviceGroup));
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &DevicesService::SetDeviceGroup));
    CROW_ROUTE(app, API_CLIENT_DEVICE_GET_INFO).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &DevicesService::GetDeviceInfo));
    CROW_ROUTE(app, API_DEVICE).methods(crow::HTTPMethod::DELETE)(BaseService::bind(this, &DevicesService::DeleteDevice));
    CROW_ROUTE(app, API_CLIENT_RESTART_DEVICE).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &DevicesService::RestartDevice));
}

crow::response DevicesService::ListDevices() const {
    nlohmann::json result;
    try {
        std::vector<std::vector<std::string>> data;
        static const std::string query = "SELECT * FROM Devices";
        if (_queryExecutor->Select(query, data))
            result = DbExtension::CreateVectorFromDbStrings<ExtendedComponentDescription>(data);
        else
            LOG_SQL_ERROR(query);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DevicesService::ListDevices");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, result.dump());
}

crow::response DevicesService::GetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field) const {
    nlohmann::json result;
    try {
        const std::string query = fmt::format("SELECT {} FROM Devices WHERE id = '{}'", field, idString);
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data)) {
            if (data.size() == 1) {
                DeviceProperty deviceProperty;
                deviceProperty._value = data[0][1];
                result = deviceProperty;
            } else {
                if (data.size() == 0)
                    LOG_ERROR_MSG(fmt::format("No devices with id {}", idString));
                else
                    LOG_ERROR_MSG(fmt::format("Too many devices with same id {}", idString));
            }
        } else
            LOG_SQL_ERROR(query);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DevicesService::GetDeviceProperty");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, result.dump());
}

crow::response DevicesService::GetDeviceName(const crow::request& request, const std::string& idString) const {
    return GetDeviceProperty(request, idString, "name");
}

crow::response DevicesService::GetDeviceGroup(const crow::request& request, const std::string& idString) const {
    return GetDeviceProperty(request, idString, "grp");
}

crow::response
DevicesService::SetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field, bool canBeEmpty) {
    try {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto deviceProperty = bodyJson.get<DeviceProperty>();
        if (canBeEmpty || !deviceProperty._value.empty()) {
            const std::string query = fmt::format("UPDATE Devices SET {} = '{}' WHERE id = '{}'", field, deviceProperty._value, idString);
            if (_queryExecutor->Execute(query))
                return crow::response(crow::OK);
            else
                LOG_SQL_ERROR(query);
        } else
            LOG_ERROR_MSG(fmt::format("Invalid device {} {}", field, request.body));
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DevicesService::SetDeviceProperty");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::SetDeviceName(const crow::request& request, const std::string& idString) {
    return SetDeviceProperty(request, idString, "name", false);
}

crow::response DevicesService::SetDeviceGroup(const crow::request& request, const std::string& idString) {
    return SetDeviceProperty(request, idString, "grp", true);
}

crow::response DevicesService::GetDeviceInfo(const crow::request& request) {

    nlohmann::json result;
    try {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromRequest(request);
        result = CallProcessorsJsonResult(timestamp, message);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DevicesService::GetDeviceInfo");
    }
    return crow::response(crow::OK, result.dump());
}

crow::response DevicesService::DeleteDevice(const std::string& idString) {
    try {
        // delete device from all tables
        for (const auto& table : _queryExecutor->GetDeviceRelatedTables()) {
            const std::string query = fmt::format("DELETE FROM {} WHERE id = '{}'", table, idString);
            if (!_queryExecutor->Delete(query)) {
                LOG_ERROR_MSG(fmt::format("Failed to delete device {} from {}", idString, table));
                return crow::response(crow::BAD_REQUEST);
            }
        }
        {
            SimpleTableDeleteInput what{
                ._id = Uuid{ idString },
            };
            if (GetSettingsCache(_queryExecutor)->Delete(what)._type != StorageCacheProblemType::NoProblems) {

                LOG_ERROR_MSG(fmt::format("Failed to delete device {} from Settings", idString));
                return crow::response(crow::BAD_REQUEST);
            }
            if (GetCommandsCache(_queryExecutor)->Delete(what)._type != StorageCacheProblemType::NoProblems) {
                LOG_ERROR_MSG(fmt::format("Failed to delete device {} from Commands", idString));
                return crow::response(crow::BAD_REQUEST);
            }
        }
        // TODO: Clean event table. Right now deleted id is not a column of event table...
        return crow::response{
            crow::OK,
        };
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DevicesService::DeleteDevice");
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response DevicesService::RestartDevice(const std::string& idString) {
    auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid{ idString });
    if (connection) {
        LOG_INFO_MSG(fmt::format("Restarting device {}", idString));
        connection->send_text("{ \"restart\" : true }");
        return crow::response(crow::OK);
    }
    LOG_ERROR_MSG(fmt::format("Failed to restart device {} - not connected", idString));
    return crow::response(crow::BAD_REQUEST);
}
