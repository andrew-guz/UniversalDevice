#include "DeviceService.h"

#include "Defines.h"
#include "Logger.h"
#include "TimeHelper.h"
#include "ProcessorsFactory.h"

DeviceService::DeviceService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void DeviceService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::GET)([&](const std::string& idString){ return GetSettings(idString); });
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetSettings(idString, request); });
    CROW_ROUTE(app, API_DEVICE_INFORM).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return Inform(request); });
}

std::string DeviceService::GetSettings(const std::string& idString)
{
    try
    {
        std::stringstream queryStream;
        queryStream << "SELECT settings FROM Settings WHERE id = '"
            << idString
            << "'";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
        {
            if (data.size() == 1)
            {
                auto settingsString = data[0][1];
                if (!settingsString.empty())
                    return settingsString;
                else
                {
                    LOG_INFO << "Empty settings for device " << idString << "." << std::endl;
                    return {};
                }
            }
            else
            {
                if (data.size() == 0)
                    LOG_INFO << "No settings for device " << idString << "." << std::endl;
                else
                    LOG_ERROR << "Too many settings for device " << idString << "." << std::endl;
            }
        }
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::GetSettings." << std::endl;
    } 
    return {};
}

int DeviceService::SetSettings(const std::string& idString, const crow::request& request)
{
    try
    {
        auto settingsString = request.body;
        if (!settingsString.empty())
        {
            std::stringstream queryStream;
            queryStream << "INSERT OR REPLACE INTO Settings (id, settings) VALUES ('"
                << idString
                << "', '"
                << settingsString
                << "')";
            queryStream.flush();
            if (_queryExecutor->Execute(queryStream.str()))
                return crow::OK;
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        else
            LOG_ERROR << "Invalid settings " << settingsString << "." << std::endl;    
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::SetSettings." << std::endl;
    } 
    return crow::BAD_REQUEST;
}

int DeviceService::Inform(const crow::request& request)
{
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromRequest(request);
        CallProcessorsNoResult(timestamp, message);
        return crow::OK;
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::Inform." << std::endl;
    }
    return crow::BAD_REQUEST;
}
