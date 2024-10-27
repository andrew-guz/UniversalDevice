#include "MainService.hpp"

#include <fmt/format.h>

#include "Version.hpp"

MainService::MainService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void MainService::Initialize(CrowApp& app) {
    _application = &app;
    CROW_ROUTE(app, API_VERSION).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &MainService::GetVersion));
    CROW_ROUTE(app, API_QUIT).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &MainService::Quit));
}

crow::response MainService::GetVersion() const {
    return crow::response(crow::OK, fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH));
}

crow::response MainService::Quit() {
    _application->stop();
    return crow::response(crow::OK);
}
