#include "ClientService.h"

#include <filesystem>
#include <nlohmann/json.hpp>

#include "Defines.h"
#include "JsonExtension.h"
#include "MessageHelper.h"
#include "ProcessorsFactory.h"
#include "DeviceProperty.h"
#include "ExtendedComponentDescription.h"
#include "Event.h"
#include "StorageCacheFactory.h"
#include "EventTableStorageCache.h"
#include "LogInformation.h"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void ClientService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_CLIENT_DEVICES).methods(crow::HTTPMethod::GET)([&](const crow::request& request){ return ListDevices(request); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString){ return GetDeviceProperty(request, idString, "name"); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetDeviceProperty(request, idString, "name", false); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString){ return GetDeviceProperty(request, idString, "grp"); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GROUP).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetDeviceProperty(request, idString, "grp", true); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GET_INFO).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return GetDeviceInfo(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::GET)([&](const crow::request& request){ return GetEvents(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return AddEvent(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::PUT)([&](const crow::request& request){ return UpdateEvent(request); });
    CROW_ROUTE(app, API_CLIENT_LOGS).methods(crow::HTTPMethod::GET)([&](const crow::request& request){ return ListLogs(request); });
}

crow::response ClientService::ListDevices(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    nlohmann::json result;
    try
    {
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select("SELECT * FROM Devices", data))
        {
            auto extendedComponentDescriptions = DbExtension::CreateVectorFromDbStrings<ExtendedComponentDescription>(data);
            for (auto& extendedComponentDescription : extendedComponentDescriptions)
                result.push_back(extendedComponentDescription.ToJson());
        }
        else
            LOG_SQL_ERROR("SELECT * FROM Devices");
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::ListDevices." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::GetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    nlohmann::json result;
    try
    {
        std::stringstream queryStream;
        queryStream
            << "SELECT "
            << field
            << " FROM Devices WHERE id = '"
            << idString
            << "'";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
        {
            if (data.size() == 1)
            {
                DeviceProperty deviceProperty;
                deviceProperty._value = data[0][1];
                result = deviceProperty.ToJson();
            }
            else
            {
                if (data.size() == 0)
                    LOG_ERROR << "No devices with id " << idString << "." << std::endl;
                else
                    LOG_ERROR << "Too many devices with same id " << idString << "." << std::endl;
            }
        }
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::GetDeviceProperty." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    } 
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::SetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field, bool canBeEmpty)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto deviceProperty = JsonExtension::CreateFromJson<DeviceProperty>(bodyJson);
        if (canBeEmpty ||
            !deviceProperty._value.empty())
        {
            std::stringstream queryStream;
            queryStream
                << "UPDATE Devices SET "
                << field
                << " = '"
                << deviceProperty._value
                << "' WHERE id = '"
                << idString
                << "'";
            queryStream.flush();
            if (_queryExecutor->Execute(queryStream.str()))
                return crow::response(crow::OK);
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        else
            LOG_ERROR << "Invalid device " << field << " " << request.body << "." << std::endl;    
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::SetDeviceProperty." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    } 
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::GetDeviceInfo(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    nlohmann::json result;
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromRequest(request);
        result = CallProcessorsJsonResult(timestamp, message);
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::GetDeviceInfo." << std::endl;
    } 
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::GetEvents(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    nlohmann::json result = nlohmann::json::array({});
    try
    {
        std::vector<std::string> eventStrings;

        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<EventTableStorageCache, false>(_queryExecutor, "Events");
        EventTableSelectAllOutput eventsResult;
        auto problem = storageCache->SelectAll(eventsResult);
        switch(problem._type)
        {
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

        for (auto& eventString : eventStrings)
        {
            try
            {
                nlohmann::json eventJson = nlohmann::json::parse(eventString);
                result.push_back(eventJson);
            }
            catch(...)
            {
                LOG_ERROR << "Invalid event JSON " << eventString << "." << std::endl;
            }
        }
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::GetEvents." << std::endl;
    } 
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::AddEvent(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto event = JsonExtension::CreateFromJson<Event>(bodyJson);

        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<EventTableStorageCache, false>(_queryExecutor, "Events");
        EventTableAddInput what;
        what._id = event._id.data();
        what._active = event._active;
        what._providerDescription = event._provider;
        what._event = request.body;
        auto problem = storageCache->Add(what);
        switch(problem._type)
        {
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
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::DeleteEvent." << std::endl;
    } 
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::UpdateEvent(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto event = JsonExtension::CreateFromJson<Event>(bodyJson);

        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<EventTableStorageCache, false>(_queryExecutor, "Events");
        EventTableUpdateInput what;
        what._id = event._id.data();
        what._active = event._active;
        what._providerDescription = event._provider;
        what._event = request.body;
        auto problem = storageCache->Update(what);
        switch(problem._type)
        {
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
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::UpdateEvent." << std::endl;
    } 
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::DeleteEvent(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto event = JsonExtension::CreateFromJson<Event>(bodyJson);

        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<EventTableStorageCache, false>(_queryExecutor, "Events");
        EventTableDeleteInput what;
        what._id = event._id.data();
        auto problem = storageCache->Delete(what);
        switch(problem._type)
        {
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
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::DeleteEvent." << std::endl;
    } 
    return crow::response(crow::BAD_REQUEST);
}

crow::response ClientService::ListLogs(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    nlohmann::json result = nlohmann::json::array({});
    auto logDir = PathHelper::AppDirPath();
    for (auto entry : std::filesystem::directory_iterator(logDir))
    {
        if (entry.path().extension() == ".log")
        {
            LogInformation logInformation;
            logInformation._fileName = entry.path().filename().string();
            std::ifstream logStream(entry.path().string());
            std::ostringstream logBuffer;
            logBuffer << logStream.rdbuf();
            logInformation._fileContent = logBuffer.str();
            result.push_back(logInformation.ToJson());
        }
    }
    return crow::response(crow::OK, result.dump());
}
