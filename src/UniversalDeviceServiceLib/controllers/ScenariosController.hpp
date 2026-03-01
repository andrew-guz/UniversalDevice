#pragma once

#include <optional>
#include <vector>

#include "Cache.hpp"
#include "CommandsController.hpp"
#include "Controller.hpp"
#include "EventsController.hpp"
#include "IQueryExecutor.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"

class ScenariosController final : public Controller {
public:
    ScenariosController(IQueryExecutor* queryExecutor, EventsController& eventsController, CommandsController& commandsController);

    ~ScenariosController() = default;

    std::vector<Scenario> List() const;

    std::optional<Scenario> Get(const Uuid& id) const;

    bool Add(const Scenario& scenario);

    bool Update(const Scenario& scenario);

    void CleanupScenario(Scenario& scenario);

    bool ActivateScenario(const Uuid& id);

    bool Remove(const Uuid& id);

private:
    void FillCache() const;

private:
    mutable Cache<Uuid, Scenario> _cache;
    EventsController& _eventsController;
    CommandsController& _commandsController;
};
