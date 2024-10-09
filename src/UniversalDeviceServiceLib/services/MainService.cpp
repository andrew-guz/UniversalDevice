#include "MainService.hpp"

#include <fmt/format.h>

#include "Version.hpp"

MainService::MainService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void MainService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_VERSION).methods(crow::HTTPMethod::GET)([]() {
        return crow::response(crow::OK, fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH));
    });
    CROW_ROUTE(app, API_QUIT).methods(crow::HTTPMethod::GET)([&app]() {
        app.stop();
        return crow::response(crow::OK);
    });
}
