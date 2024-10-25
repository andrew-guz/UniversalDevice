#pragma once

#include "BaseService.hpp"

class ScenariosService final : public BaseService {
protected:
    ScenariosService(IQueryExecutor* queryExecutor);

public:
    virtual ~ScenariosService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetScenarios() const;

    crow::response AddScenario(Scenario& scenario);

    crow::response UpdateScenario(Scenario& scenario);

    crow::response DeleteScenario(const std::string& scenarioId);

    crow::response ActivateScenario(const std::string& scenarioId);

private:
    friend class BaseServiceExtension;
};
