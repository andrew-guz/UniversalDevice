#include "ScenarioTableStorageCache.hpp"

#include "StorageCacheFactory.hpp"

ScenarioTableStorageCache::ScenarioTableStorageCache(IQueryExecutor* queryExecutor) :
    SimpleTableStorageCache<Scenario>(queryExecutor, "Scenarios", "scenarios") {}

IStorageCache* GetScenariosCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<ScenarioTableStorageCache, false>(queryExecutor, "Scenarios");
}
