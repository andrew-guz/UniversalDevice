#include "MainService.h"

#include "Defines.h"

MainService::MainService(IQueryExecutor* queryExecutor) : BaseService(queryExecutor) {}

void MainService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_VERSION).methods(crow::HTTPMethod::GET)([&](const crow::request& request) { return Version(request); });
    CROW_ROUTE(app, API_QUIT).methods(crow::HTTPMethod::GET)([&](const crow::request& request) { return Quit(request, app); });
}

crow::response MainService::Version(const crow::request& request) { return crow::response(crow::OK, VERSION); }

crow::response MainService::Quit(const crow::request& request, CrowApp& app) {
    app.stop();
    return crow::response(crow::OK);
}
