#include "ClientService.h"

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
    return "{}";
}
