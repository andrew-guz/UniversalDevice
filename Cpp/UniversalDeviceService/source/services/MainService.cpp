#include "MainService.h"

#include "Defines.h"

MainService::MainService(IQueryExecutor* queryExecutor) : BaseService(queryExecutor) {}

void MainService::Initialize(crow::SimpleApp& app) {
    CROW_ROUTE(app, API_VERSION).methods(crow::HTTPMethod::GET)([&](const crow::request& request) { return Version(request); });
    CROW_ROUTE(app, API_QUIT).methods(crow::HTTPMethod::GET)([&](const crow::request& request) { return Quit(request, app); });
}

crow::response MainService::Version(const crow::request& request) {
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    return crow::response(crow::OK, VERSION);
}

crow::response MainService::Quit(const crow::request& request, crow::SimpleApp& app) {
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    app.stop();
    return crow::response(crow::OK);
}
