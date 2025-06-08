#include "PostgreSQLStorage.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "fmt/format.h"
#include "pqxx/internal/statement_parameters.hxx"
#include <pqxx/pqxx>

#include "Storage.hpp"

PostgreSQLStorage::PostgreSQLStorage(std::string_view dbName, std::string_view username, std::string_view password) :
    Storage(),
    connection(fmt::format("hostaddr=127.0.0.1 port=5432 dbname={} user={} password={}", dbName, username, password)) //
{
    InitializeDb();
}

PostgreSQLStorage::~PostgreSQLStorage() { Commit(); }

bool PostgreSQLStorage::Begin() {
    if (transaction)
        return false;

    transaction = std::make_shared<pqxx::work>(this->connection);
    return true;
}

bool PostgreSQLStorage::Execute(const std::string_view query) {

    try {
        pqxx::work transaction(this->connection);

        transaction.exec(query);
        transaction.commit();

        return true;
    } catch (const pqxx::sql_error& sqlError) {
        LOG_ERROR_MSG(fmt::format("SQL error: {} for query {}", sqlError.what(), sqlError.query()));
    } catch (...) {
        LOG_ERROR_MSG("SQL error");
    }

    return false;
}

bool PostgreSQLStorage::Select(const std::string_view query, std::vector<std::vector<std::string>>& data) {
    try {
        pqxx::work transaction(this->connection);

        const pqxx::result result = transaction.exec(query);
        for (const pqxx::row& row : result) {
            std::vector<std::string> rowResult;
            for (const auto& value : row) {
                rowResult.push_back(value.name());
                rowResult.push_back(value.c_str());
            }
            data.emplace_back(std::move(rowResult));
        }

        transaction.commit();

        return true;
    } catch (const pqxx::sql_error& sqlError) {
        LOG_ERROR_MSG(fmt::format("SQL error: {} for query {}", sqlError.what(), sqlError.query()));
    } catch (...) {
        LOG_ERROR_MSG("SQL error");
    }

    return false;
}

bool PostgreSQLStorage::Commit() {
    if (!transaction)
        return false;

    try {
        transaction->commit();
        transaction.reset();

        return true;
    } catch (const pqxx::sql_error& sqlError) {
        LOG_ERROR_MSG(fmt::format("SQL error: {} for query {}", sqlError.what(), sqlError.query()));
    } catch (...) {
        LOG_ERROR_MSG("SQL error");
    }

    return false;
}
