#include "CommandsService.hpp"

#include "SimpleTableStorageCache.hpp"
#include "WebsocketsCache.hpp"

CommandsService::CommandsService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void CommandsService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &CommandsService::GetCommands));
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &CommandsService::SetCommands));
}

crow::response CommandsService::GetCommands(const std::string& idString) const {
    try {
        auto storageCache = GetCommandsCache(_queryExecutor);
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
                LOG_INFO_MSG(fmt::format("Empty commands for device {}", idString));
                break;
            case StorageCacheProblemType::NotExists:
                LOG_DEBUG_MSG(fmt::format("No commands for device {}", idString));
                break;
            case StorageCacheProblemType::TooMany:
                LOG_ERROR_MSG(fmt::format("Too many commands for device {}", idString));
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in CommandsService::GetCommands");
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, std::string());
}

crow::response CommandsService::SetCommands(const crow::request& request, const std::string& idString) {
    try {
        auto commandsString = request.body;
        auto storageCache = GetCommandsCache(_queryExecutor);
        SimpleTableInsertOrReplaceInput what{
            ._id = Uuid{ idString },
            ._data = commandsString,
        };
        auto problem = storageCache->InsertOrReplace(what);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems: {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
                if (connection)
                    connection->send_text(commandsString);
                return crow::response(crow::OK);
            } break;
            case StorageCacheProblemType::Empty:
                LOG_ERROR_MSG(fmt::format("Invalid commands {}", commandsString));
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
        LOG_ERROR_MSG("Something went wrong in CommandsService::SetCommands");
    }
    return crow::response(crow::BAD_REQUEST);
}
