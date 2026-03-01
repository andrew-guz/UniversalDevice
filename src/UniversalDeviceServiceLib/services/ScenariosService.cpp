#include "ScenariosService.hpp"

#include <string>

#include <crow/app.h>
#include <crow/common.h>
#include <crow/http_response.h>
#include <nlohmann/json_fwd.hpp>

#include "BaseService.hpp"
#include "Defines.hpp"
#include "EventsController.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Middleware.hpp"
#include "Scenario.hpp"
#include "ScenariosController.hpp"
#include "Uuid.hpp"

ScenariosService::ScenariosService(CrowApp& app, ScenariosController& scenariosController, EventsController& eventsController) :
    _scenariosController(scenariosController),
    _eventsController(eventsController) //
{
    CROW_ROUTE(app, API_CLIENT_SCENARIOS).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &ScenariosService::GetScenarios));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS)
        .methods(crow::HTTPMethod::POST)(ServiceExtension::bindObject(this, &ScenariosService::AddScenario, "AddScenario"));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS)
        .methods(crow::HTTPMethod::PUT)(ServiceExtension::bindObject(this, &ScenariosService::UpdateScenario, "UpdateScenario"));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS_ID).methods(crow::HTTPMethod::DELETE)(ServiceExtension::bind(this, &ScenariosService::DeleteScenario));
    CROW_ROUTE(app, API_CLIENT_SCENARIOS_ID).methods(crow::HTTPMethod::PATCH)(ServiceExtension::bind(this, &ScenariosService::ActivateScenario));
}

crow::response ScenariosService::GetScenarios() const {
    nlohmann::json result;

    try {
        result = _scenariosController.List();
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ScenariosService::GetScenarios");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK, result.dump());
}

crow::response ScenariosService::AddScenario(Scenario& scenario) {
    try {
        _scenariosController.CleanupScenario(scenario);

        if (!_scenariosController.Add(scenario)) {
            LOG_ERROR_MSG("Failed to add scenario");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::AddScenario");
        return crow::response{ crow::BAD_REQUEST };
    }

    return crow::response{ crow::OK };
}

crow::response ScenariosService::UpdateScenario(Scenario& scenario) {
    try {
        _scenariosController.CleanupScenario(scenario);

        if (!_scenariosController.Add(scenario)) {
            LOG_ERROR_MSG("Failed to update scenario");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::UpdateScenario");
        return crow::response{ crow::BAD_REQUEST };
    }

    return crow::response{ crow::OK };
}

crow::response ScenariosService::DeleteScenario(const std::string& scenarioId) {
    try {
        if (!_scenariosController.Remove(Uuid{ scenarioId })) {
            LOG_ERROR_MSG("Failed to remove scenario");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::DeleteScenario");
        return crow::response{ crow::BAD_REQUEST };
    }

    return crow::response{ crow::OK };
}

crow::response ScenariosService::ActivateScenario(const std::string& scenarioId) {
    try {
        if (!_scenariosController.ActivateScenario(Uuid{ scenarioId })) {
            LOG_ERROR_MSG("Failed to activate scenario");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::ActivateScenario");
        return crow::response{ crow::BAD_REQUEST };
    }

    return crow::response{ crow::OK };
}
