#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <pqxx/pqxx>

#include "Storage.hpp"

class PostgreSQLStorage final : public Storage {
    PostgreSQLStorage(std::string_view dbName, std::string_view username, std::string_view password);

    virtual ~PostgreSQLStorage();

    virtual bool Begin() override;

    virtual bool Execute(std::string_view query) override;

    virtual bool Select(std::string_view query, std::vector<std::vector<std::string>>& data) override;

    virtual bool Commit() override;

protected:
    pqxx::connection connection;
    std::shared_ptr<pqxx::work> transaction;
};
