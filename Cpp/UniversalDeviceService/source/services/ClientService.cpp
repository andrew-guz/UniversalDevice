#include "ClientService.h"

#include <nlohmann/json.hpp>

#include "Defines.h"
#include "MessageHelper.h"
#include "ProcessorsFactory.h"
#include "DeviceName.h"
#include "ExtendedComponentDescription.h"
#include "Event.h"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void ClientService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_CLIENT_LIST_DEVICES).methods(crow::HTTPMethod::GET)([&](const crow::request& request){ return ListDevices(request); });
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
        queryStream << "SELECT name FROM Devices WHERE id = '"
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
            queryStream << "UPDATE Devices SET name = '"
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
    nlohmann::json result;
    try
    {
        std::vector<std::vector<std::string>> data;
        std::stringstream queryStream;
        queryStream << "SELECT event FROM Events";
        queryStream.flush();
        if (_queryExecutor->Select(queryStream.str(), data))
        {
            for (auto& row : data)
            {
                auto eventString = DbExtension::FindValueByName(row, "event");
                if (eventString.empty())
                {
                    LOG_ERROR << "Empty event found." << std::endl;
                    continue;
                }
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
        else
            LOG_SQL_ERROR(queryStream.str());
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
        std::stringstream queryStream;
        queryStream << "INSERT INTO Events (id, active, providerId, providerType, event) VALUES ('"
            << event._id.data()
            << "', "
            << (event._active ? "1" : "0")
            << ", '"
            << event._provider._id.data()
            << "', '"
            << event._provider._type
            << "', '"
            << request.body
            << "')";
        queryStream.flush();
        if (_queryExecutor->Execute(queryStream.str()))
            return crow::response(crow::OK);
        else
            LOG_SQL_ERROR(queryStream.str());
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
        std::stringstream queryStream;
        queryStream << "UPDATE Events SET active = "
            << (event._active ? "1" : "0")
            << ", providerId = '"
            << event._provider._id.data()
            << "', providerType = '"
            << event._provider._type
            << "', event = '"
            << request.body
            << "' WHERE id = '"
            << event._id.data()
            << "'";
        queryStream.flush();
        if (_queryExecutor->Execute(queryStream.str()))
            return crow::response(crow::OK);
        else
            LOG_SQL_ERROR(queryStream.str());
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
        std::stringstream queryStream;
        queryStream << "DELETE FROM Events WHERE id='"
            << event._id.data()
            << "'";
        queryStream.flush();
        if (_queryExecutor->Execute(queryStream.str()))
            return crow::response(crow::OK);
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::DeleteEvent." << std::endl;
    } 
    return crow::response(crow::BAD_REQUEST);
}
