#include "MainService.h"

#include "Defines.h"

MainService::MainService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void MainService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, "/api/version")([&](){ return Version(); });
    CROW_ROUTE(app, "/api/quit")([&](){ return Quit(app); });
}

std::string MainService::Version()
{
    return VERSION;
}

int MainService::Quit(crow::SimpleApp& app)
{
    app.stop();
    return crow::OK;
}
