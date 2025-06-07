#include "PostgreSQLStorage.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Storage.hpp"

PostgreSQLStorage::PostgreSQLStorage() :
    Storage(),
    connection("hostaddr=127.0.0.1 port=5432 dbname=universal_device user=test password=test") {}

PostgreSQLStorage::~PostgreSQLStorage() {
    if (transaction)
        transaction->commit();
}

bool PostgreSQLStorage::Begin() {
    if (transaction)
        return false;

    transaction = std::make_shared<pqxx::work>(this->connection);
    return true;
}

bool PostgreSQLStorage::Execute(const std::string_view query) { return false; }

bool PostgreSQLStorage::Execute(const std::string_view query, int (*callback)(void*, int, char**, char**)) { return false; }

bool PostgreSQLStorage::Select(const std::string_view query, std::vector<std::vector<std::string>>& data) { return false; }

bool PostgreSQLStorage::Delete(const std::string_view query) { return false; }

bool PostgreSQLStorage::Commit() {
    if (!transaction)
        return false;

    transaction->commit();
    transaction.reset();
    return true;
}

void PostgreSQLStorage::CleanupOldData(const std::chrono::system_clock::time_point& timestamp) {}
