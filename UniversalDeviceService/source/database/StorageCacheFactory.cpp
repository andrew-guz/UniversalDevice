#include "StorageCacheFactory.h"

std::string StorageCacheFactory::CreateKey(const std::string& tableName, const std::string& fieldName) { return tableName + std::string("/") + fieldName; }
