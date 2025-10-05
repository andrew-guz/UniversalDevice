#include "BackendLogsService.hpp"

#include "BaseService.hpp"
#include "FileUtils.hpp"

BackendLogsService::BackendLogsService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void BackendLogsService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_LOGS).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &BackendLogsService::GetBackendLog));
    CROW_ROUTE(app, API_CLIENT_LOGS).methods(crow::HTTPMethod::DELETE)(ServiceExtension::bind(this, &BackendLogsService::ClearBackendLog));
}

crow::response BackendLogsService::GetBackendLog() const {
    const LogInformation logInformation = ReadApplicationLogFile();
    return crow::response(crow::OK, static_cast<nlohmann::json>(logInformation).dump());
}

crow::response BackendLogsService::ClearBackendLog() const {
    Logger::Cleanup();
    return crow::response{ crow::OK };
}
