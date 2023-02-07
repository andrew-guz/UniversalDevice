#include "ClientService.h"

#include <nlohmann/json.hpp>

#include "Defines.h"
#include "MessageHelper.h"
#include "ProcessorsFactory.h"
#include "DeviceName.h"
#include "ExtendedComponentDescription.h"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void ClientService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_CLIENT_LIST_DEVICES).methods(crow::HTTPMethod::GET)([&](){ return ListDevices(); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::GET)([&](const std::string& idString){ return GetDeviceName(idString); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_NAME).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetDeviceName(idString, request); });
    CROW_ROUTE(app, API_CLIENT_DEVICE_GET_INFO).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return GetDeviceInfo(request); });
}

std::string ClientService::ListDevices()
{
    nlohmann::json result;
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select("SELECT * FROM Devices", data))
    {
        auto extendedComponentDescriptions = DbExtension::CreateVectorFromDbStrings<ExtendedComponentDescription>(data);
        for (auto& extendedComponentDescription : extendedComponentDescriptions)
            result.push_back(extendedComponentDescription.ToJson());
    }
    else
        LOG_SQL_ERROR("SELECT * FROM Devices");
    return result.dump();
}

std::string ClientService::GetDeviceName(const std::string& idString)
{
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
    } 
    return result.dump();
}

int ClientService::SetDeviceName(const std::string& idString, const crow::request& request)
{
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
                return crow::OK;
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        else
            LOG_ERROR << "Invalid device name " << request.body << "." << std::endl;    
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in ClientService::GetDeviceName." << std::endl;
    } 
    return crow::BAD_REQUEST;
}

std::string ClientService::GetDeviceInfo(const crow::request& request)
{
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
    return result.dump();
}
