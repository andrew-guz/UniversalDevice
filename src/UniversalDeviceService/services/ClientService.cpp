#include "ClientService.hpp"

#include <filesystem>
#include <string>

#include "nlohmann/json_fwd.hpp"
#include <fmt/format.h>

#include "Defines.hpp"
#include "DeviceProperty.hpp"
#include "EventTableStorageCache.hpp"
#include "ExtendedComponentDescription.hpp"
#include "FileUtils.hpp"
#include "LogInformation.hpp"
#include "Marshaling.hpp"
#include "PathHelper.hpp"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void ClientService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_DEVICES).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &ClientService::ListDevices));
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString) {
        return GetDeviceProperty(request, idString, "name");
    });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString) {
        return SetDeviceProperty(request, idString, "name", false);
    });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString) {
        return GetDeviceProperty(request, idString, "grp");
    });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString) {
        return SetDeviceProperty(request, idString, "grp", true);
    });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GET_INFO).methods(crow::HTTPMethod::POST)([&](const crow::request& request) { return GetDeviceInfo(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &ClientService::GetEvents));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::POST)(BaseService::bindObject(this, &ClientService::AddEvent, "AddEvent"));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::PUT)(BaseService::bindObject(this, &ClientService::UpdateEvent, "UpdateEvent"));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::DELETE)(BaseService::bindObject(this, &ClientService::DeleteEvent, "DeleteEvent"));
    CROW_ROUTE(app, API_CLIENT_LOGS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &ClientService::GetBackendLog));
}

crow::response ClientService::ListDevices() {
    nlohmann::json result;
    try {
        std::vector<std::vector<std::string>> data;
        static const std::string query = "SELECT * FROM Devices";
        if (_queryExecutor->Select(query, data))
            result = DbExtension::CreateVectorFromDbStrings<ExtendedComponentDescription>(data);
        else
            LOG_SQL_ERROR(query);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::ListDevices");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::GetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field) {
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
        LOG_ERROR_MSG("Something went wrong in ClientService::GetDeviceProperty");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, result.dump());
}

crow::response
ClientService::SetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field, bool canBeEmpty) {
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
        LOG_ERROR_MSG("Something went wrong in ClientService::SetDeviceProperty");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::GetDeviceInfo(const crow::request& request) {

    nlohmann::json result;
    try {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromRequest(request);
        result = CallProcessorsJsonResult(timestamp, message);
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::GetDeviceInfo");
    }
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::GetEvents() {
    nlohmann::json result = nlohmann::json::array({});
    try {
        std::vector<std::string> eventStrings;

        auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
        EventTableSelectAllOutput eventsResult;
        auto problem = storageCache->SelectAll(eventsResult);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                eventStrings = eventsResult._data;
                break;
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }

        for (auto& eventString : eventStrings) {
            try {
                nlohmann::json eventJson = nlohmann::json::parse(eventString);
                result.push_back(eventJson);
            } catch (...) {
                LOG_ERROR_MSG(fmt::format("Invalid event JSON {}", eventString));
            }
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::GetEvents");
    }
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::AddEvent(const Event& event, const std::string& eventString) {
    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableInsertOrReplaceInput what{
        ._id = event._id,
        ._active = event._active,
        ._providerId = event._provider._id,
        ._providerType = event._provider._type,
        ._event = eventString,
    };
    auto problem = storageCache->InsertOrReplace(what);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK);
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::UpdateEvent(const Event& event, const std::string& eventString) {
    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableUpdateInput what{
        ._id = event._id,
        ._active = event._active,
        ._providerId = event._provider._id,
        ._providerType = event._provider._type,
        ._event = eventString,
    };
    auto problem = storageCache->Update(what);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK);
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::DeleteEvent(const Event& event) {
    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableDeleteInput what{
        ._id = event._id,
    };
    auto problem = storageCache->Delete(what);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK);
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::GetBackendLog() {
    const LogInformation logInformation = ReadApplicationLogFile();
    return crow::response(crow::OK, static_cast<nlohmann::json>(logInformation).dump());
}
