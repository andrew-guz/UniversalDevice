#include "SimpleTableStorageCache.hpp"

#include "StorageCacheFactory.hpp"

IStorageCache* GetSettingsCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache<std::string>>(queryExecutor, "Settings", "settings");
}

IStorageCache* GetCommandsCache(IQueryExecutor* queryExecutor) {
    return StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache<std::string>>(queryExecutor, "Commands", "commands");
}
