#include "ClientService.h"

#include <nlohmann/json.hpp>

#include "Defines.h"
#include "JsonExtension.h"
#include "MessageHelper.h"
#include "ProcessorsFactory.h"
#include "DeviceName.h"
#include "ExtendedComponentDescription.h"
#include "Event.h"
#include "StorageCacheFactory.h"
#include "EventTableStorageCache.h"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void ClientService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_CLIENT_DEVICES).methods(crow::HTTPMethod::GET)([&](const crow::request& request){ return ListDevices(request); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString){ return GetDeviceName(request, idString); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetDeviceName(request, idString); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GET_INFO).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return GetDeviceInfo(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::GET)([&](const crow::request& request){ return GetEvents(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return AddEvent(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::PUT)([&](const crow::request& request){ return UpdateEvent(request); });
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::DELETE)([&](const crow::request& request){ return DeleteEvent(request); });
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

crow::response ClientService::GetDeviceName(const crow::request& request, const std::string& idString)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    nlohmann::json result;
    try
    {
        std::stringstream queryStream;
        queryStream
            << "SELECT name FROM Devices WHERE id = '"
            << idString
            << "'";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
        {
            if (data.size() == 1)
            {
                DeviceName deviceName;
                deviceName._name = data[0][1];
                result = deviceName.ToJson();
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
        LOG_ERROR << "Something went wrong in ClientService::GetDeviceName." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    } 
    return crow::response(crow::OK, result.dump());
}

crow::response ClientService::SetDeviceName(const crow::request& request, const std::string& idString)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto bodyJson = nlohmann::json::parse(request.body);
        auto deviceName = JsonExtension::CreateFromJson<DeviceName>(bodyJson);
        if (!deviceName._name.empty())
        {
            std::stringstream queryStream;
            queryStream
                << "UPDATE Devices SET name = '"
                << deviceName._name
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
            LOG_ERROR << "Invalid device name " << request.body << "." << std::endl;    
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::GetDeviceName." << std::endl;
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
        EventTableInsertOrReplaceInput what;
        what._id = event._id.data();
        what._active = event._active;
        what._providerId = event._provider._id.data();
        what._providerType = event._provider._type;
        what._event = request.body;
        auto problem = storageCache->InsertOrReplace(what);
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
        what._providerId = event._provider._id.data();
        what._providerType = event._provider._type;
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
