#include "BackendLogsService.hpp"

#include <crow/app.h>
#include <crow/common.h>
#include <crow/http_response.h>
#include <nlohmann/json_fwd.hpp>

#include "BaseService.hpp"
#include "Defines.hpp"
#include "FileUtils.hpp"
#include "LogInformation.hpp"
#include "Logger.hpp"
#include "Middleware.hpp"

BackendLogsService::BackendLogsService(CrowApp& app) {
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
