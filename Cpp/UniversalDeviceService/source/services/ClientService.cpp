#include "ClientService.h"

#include <nlohmann/json.hpp>

#include "Defines.h"
#include "MessageHelper.h"
#include "ProcessorsFactory.h"
#include "ExtendedComponentDescription.h"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void ClientService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_CLIENT_LIST_DEVICES).methods(crow::HTTPMethod::GET)([&](){ return ListDevices(); });
    CROW_ROUTE(app, API_CLIENT_DEVICE).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return GetDeviceInfo(request); });
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
    return result.dump();
}

std::string ClientService::GetDeviceInfo(const crow::request& request)
{
    nlohmann::json result;
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromRequest(request);
        if (message.IsValid())
        {
            auto processors = ProcessorsFactory::CreateProcessors(message, _queryExecutor);
            for (auto& processor : processors)
            {
                auto processorResultJson = processor->ProcessMessage(timestamp, message);
                if(processorResultJson.is_null())
                {
                    continue;
                }
                else if(processorResultJson.is_array())
                {
                    for (auto& subResult : processorResultJson)
                        result.push_back(subResult);
                }
                else
                    result.push_back(processorResultJson);
            }
        }
        else
        {
            LOG_ERROR << "Invalid message." << std::endl;
        }
    }
    catch(...)
    {
        //do nothing
    } 
    return result.dump();
}
