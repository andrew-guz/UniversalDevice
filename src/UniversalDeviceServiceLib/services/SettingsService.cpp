#include "SettingsService.hpp"

#include "SimpleTableStorageCache.hpp"
#include "WebsocketsCache.hpp"

SettingsService::SettingsService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void SettingsService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &SettingsService::GetSettings));
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &SettingsService::SetSettings));
}

crow::response SettingsService::GetSettings(const std::string& idString) const {
    try {
        auto storageCache = GetSettingsCache(_queryExecutor);
        SimpleTableSelectInput what{
            ._id = Uuid{ idString },
        };
        SimpleTableSelectOutput<std::string> result;
        auto problem = storageCache->Select(what, result);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                return crow::response(crow::OK, result._data);
                break;
            case StorageCacheProblemType::Empty:
                LOG_INFO_MSG(fmt::format("Empty settings for device {}", idString));
                break;
            case StorageCacheProblemType::NotExists:
                LOG_DEBUG_MSG(fmt::format("No settings for device {}", idString));
                break;
            case StorageCacheProblemType::TooMany:
                LOG_ERROR_MSG(fmt::format("Too many settings for device {}", idString));
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in SettingsService::GetSettings");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, std::string());
}

crow::response SettingsService::SetSettings(const crow::request& request, const std::string& idString) {
    try {
        auto settingsString = request.body;
        auto storageCache = GetSettingsCache(_queryExecutor);
        SimpleTableInsertOrReplaceInput what{
            ._id = Uuid{ idString },
            ._data = settingsString,
        };
        auto problem = storageCache->InsertOrReplace(what);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems: {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
                if (connection)
                    connection->send_text(settingsString);
                return crow::response(crow::OK);
            } break;
            case StorageCacheProblemType::Empty:
                LOG_ERROR_MSG(fmt::format("Invalid settings {}", settingsString));
                break;
            case StorageCacheProblemType::NotExists:
                break;
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in SettingsService::SetSettings");
    }
    return crow::response(crow::BAD_REQUEST);
}
