#pragma once

#include "Scenario.hpp"
#include "SimpleTableStorageCache.hpp"

class ScenarioTableStorageCache final : public SimpleTableStorageCache<Scenario> {
public:
    ScenarioTableStorageCache(IQueryExecutor* queryExecutor);
};

IStorageCache* GetScenariosCache(IQueryExecutor* queryExecutor);
