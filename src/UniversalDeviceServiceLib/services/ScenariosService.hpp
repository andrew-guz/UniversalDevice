#pragma once

#include <string>

#include <crow/http_response.h>

#include "BaseService.hpp"
#include "EventsController.hpp"
#include "Middleware.hpp"
#include "Scenario.hpp"
#include "ScenariosController.hpp"

class ScenariosService final : public BaseService {
public:
    ScenariosService(CrowApp& app, ScenariosController& scenariosController, EventsController& eventsController);

    virtual ~ScenariosService() = default;

private:
    crow::response GetScenarios() const;

    crow::response AddScenario(Scenario& scenario);

    crow::response UpdateScenario(Scenario& scenario);

    crow::response DeleteScenario(const std::string& scenarioId);

    crow::response ActivateScenario(const std::string& scenarioId);

private:
    ScenariosController& _scenariosController;
    EventsController& _eventsController;

private:
    friend class ServiceExtension;
};
