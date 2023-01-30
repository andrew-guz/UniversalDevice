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
        nlohmann::json replyData;
        auto extendedComponentDescriptions = ExtendedComponentDescription::CreateFromDbStrings(data);
        for (auto& extendedComponentDescription : extendedComponentDescriptions)
            replyData.push_back(extendedComponentDescription.ToJson());
        auto replyMessage = MessageHelper::Create(Constants::ClientServiceType, Uuid::Empty(), Constants::FrontendServiceType, Uuid::Empty(), Constants::SubjectGetDevicesList, replyData);
        result = replyMessage.ToJson();
    }
    return result.dump();
}

std::string ClientService::GetDeviceInfo(const crow::request& request)
{
    return {};
}
