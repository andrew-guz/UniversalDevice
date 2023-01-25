#include "ClientService.h"

#include <nlohmann/json.hpp>

#include "Defines.h"
#include "ExtendedDeviceDescription.h"

ClientService::ClientService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void ClientService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_CLIENT_LIST_DEVICES)([&](){ return ListDevices(); });
}

std::string ClientService::ListDevices()
{
    nlohmann::json result;
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select("SELECT * FROM 'Devices'", data))
    {
        auto extendedDeviceDescriptions = ExtendedDeviceDescription::CreateFromDbStrings(data);
        for (auto& extendedDeviceDescription : extendedDeviceDescriptions)
            result.push_back(extendedDeviceDescription.ToJson());
    }
    return result.dump();
}
