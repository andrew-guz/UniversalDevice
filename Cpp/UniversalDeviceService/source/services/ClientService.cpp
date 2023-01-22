#include "ClientService.h"

#include <nlohmann/json.hpp>

#include "Defines.h"

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
        for (auto& row : data)
            result.push_back({
                { "id", row[0] },
                { "type", row[1] },
                { "name", row[2] },
                { "timestamp", row[3] }
            });
    }
    return result.dump();
}
