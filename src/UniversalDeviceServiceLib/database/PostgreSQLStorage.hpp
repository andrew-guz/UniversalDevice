#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

#include "Storage.hpp"

class PostgreSQLStorage final : public Storage {
    PostgreSQLStorage();

    virtual ~PostgreSQLStorage();

    virtual bool Begin() override;

    virtual bool Execute(std::string_view query) override;

    virtual bool Execute(std::string_view query, int (*callback)(void*, int, char**, char**)) override;

    virtual bool Select(std::string_view query, std::vector<std::vector<std::string>>& data) override;

    virtual bool Delete(std::string_view query) override;

    virtual bool Commit() override;

    virtual void CleanupOldData(const std::chrono::system_clock::time_point& timestamp) override;
};
