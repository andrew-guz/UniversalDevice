#pragma once

#include "IQueryExecutor.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"

void CleanupScenario(Scenario& scenario, IQueryExecutor* queryExecutor);

bool ActivateScenario(IQueryExecutor* queryExecutor, const Uuid& scenarioId);
