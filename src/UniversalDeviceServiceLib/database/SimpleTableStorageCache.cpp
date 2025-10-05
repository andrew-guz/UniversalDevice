#include "SimpleTableStorageCache.hpp"

#include "Scenario.hpp"
#include "StorageCacheFactory.hpp"

IStorageCache* GetScenariosCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache<Scenario>>(queryExecutor, "Scenarios", "scenarios");
}
