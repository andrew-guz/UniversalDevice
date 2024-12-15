#include "ScenariosService.hpp"

#include "Marshaling.hpp"
#include "Scenario.hpp"
#include "ScenarioUtils.hpp"
#include "SimpleTableStorageCache.hpp"

ScenariosService::ScenariosService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void ScenariosService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_SCENARIOS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &ScenariosService::GetScenarios));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS)
        .methods(crow::HTTPMethod::POST)(BaseService::bindObject(this, &ScenariosService::AddScenario, "AddScenario"));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS)
        .methods(crow::HTTPMethod::PUT)(BaseService::bindObject(this, &ScenariosService::UpdateScenario, "UpdateScenario"));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS_ID).methods(crow::HTTPMethod::DELETE)(BaseService::bind(this, &ScenariosService::DeleteScenario));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS_ID).methods(crow::HTTPMethod::PATCH)(BaseService::bind(this, &ScenariosService::ActivateScenario));
}

crow::response ScenariosService::GetScenarios() const {
    try {
        auto storageCache = GetScenariosCache(_queryExecutor);
        SimpleTableSelectAllOutput<Scenario> scenariosResult;
        const StorageCacheProblem problem = storageCache->SelectAll(scenariosResult);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                return crow::response{
                    crow::OK,
                    static_cast<nlohmann::json>(scenariosResult._data).dump(),
                };
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ScenariosService::GetScenarios");
    }
    return crow::response{
        crow::OK,
        nlohmann::json::array().dump(),
    };
}

crow::response ScenariosService::AddScenario(Scenario& scenario) {
    try {
        CleanupScenario(scenario, _queryExecutor);

        IStorageCache* scenariosStorageCache = GetScenariosCache(_queryExecutor);
        const StorageCacheProblem problem = scenariosStorageCache->InsertOrReplace(SimpleTableInsertOrReplaceInput<Scenario>{
            ._id = scenario._id,
            ._data = scenario,
        });

        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                return crow::response{
                    crow::OK,
                };
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ScenariosService::AddScenario");
    }
    return crow::response{
        crow::BAD_REQUEST,
    };
}

crow::response ScenariosService::UpdateScenario(Scenario& scenario) {
    try {
        CleanupScenario(scenario, _queryExecutor);

        IStorageCache* scenariosStorageCache = GetScenariosCache(_queryExecutor);
        const StorageCacheProblem problem = scenariosStorageCache->Update(SimpleTableUpdateInput<Scenario>{
            ._id = scenario._id,
            ._data = scenario,
        });

        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                return crow::response{
                    crow::OK,
                };
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ScenariosService::UpdateScenario");
    }
    return crow::response{
        crow::BAD_REQUEST,
    };
}

crow::response ScenariosService::DeleteScenario(const std::string& scenarioId) {
    try {
        IStorageCache* scenariosStorageCache = GetScenariosCache(_queryExecutor);
        const StorageCacheProblem problem = scenariosStorageCache->Delete(SimpleTableDeleteInput{
            ._id = Uuid(scenarioId),
        });

        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                return crow::response{
                    crow::OK,
                };
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ScenariosService::DeleteScenario");
    }
    return crow::response{
        crow::BAD_REQUEST,
    };
}

crow::response ScenariosService::ActivateScenario(const std::string& scenarioId) {
    return crow::response{
        ::ActivateScenario(_queryExecutor, Uuid{ scenarioId }) ? crow::OK : crow::BAD_REQUEST,
    };
}
