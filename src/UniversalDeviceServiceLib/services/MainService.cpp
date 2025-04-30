#include "MainService.hpp"

#include <fmt/format.h>

#include "Defines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Version.hpp"

MainService::MainService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void MainService::Initialize(CrowApp& app) {
    _application = &app;
    CROW_ROUTE(app, API_VERSION).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &MainService::GetVersion));
    CROW_ROUTE(app, API_LOG_LEVEL).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &MainService::SetLogLevel));
    CROW_ROUTE(app, API_QUIT).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &MainService::Quit));
}

crow::response MainService::GetVersion() const {
    return crow::response(crow::OK, fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH));
}

crow::response MainService::SetLogLevel(const std::string& logLevel) {
    Logger::SetLogLevel(EnumFromString<LogLevel>(logLevel));
    return crow::response(crow::OK);
}

crow::response MainService::Quit() {
    _application->stop();
    return crow::response(crow::OK);
}
