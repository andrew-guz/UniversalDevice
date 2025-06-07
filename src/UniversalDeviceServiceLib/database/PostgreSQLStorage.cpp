#include "PostgreSQLStorage.hpp"

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

#include "Storage.hpp"

PostgreSQLStorage::PostgreSQLStorage() :
    Storage() {}

PostgreSQLStorage::~PostgreSQLStorage() {}

bool PostgreSQLStorage::Begin() { return false; }

bool PostgreSQLStorage::Execute(const std::string_view query) { return false; }

bool PostgreSQLStorage::Execute(const std::string_view query, int (*callback)(void*, int, char**, char**)) { return false; }

bool PostgreSQLStorage::Select(const std::string_view query, std::vector<std::vector<std::string>>& data) { return false; }

bool PostgreSQLStorage::Delete(const std::string_view query) { return false; }

bool PostgreSQLStorage::Commit() { return false; }

void PostgreSQLStorage::CleanupOldData(const std::chrono::system_clock::time_point& timestamp) {}
