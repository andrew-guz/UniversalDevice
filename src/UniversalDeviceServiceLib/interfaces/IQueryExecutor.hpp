#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

#include "Logger.hpp"

class IQueryExecutor {
public:
    IQueryExecutor() = default;

    virtual ~IQueryExecutor() = default;

    virtual bool Begin() = 0;

    virtual bool Execute(std::string_view query) = 0;

    virtual bool Execute(std::string_view query, int (*callback)(void*, int, char**, char**)) = 0;

    virtual bool Select(std::string_view query, std::vector<std::vector<std::string>>& data) = 0;

    virtual bool Delete(std::string query) = 0;

    virtual bool Commit() = 0;

    virtual std::vector<std::string> GetAllTables() const = 0;

    virtual std::vector<std::string> GetDeviceRelatedTables() const = 0;

    virtual std::vector<std::string> GetDataTables() const = 0;

    virtual void CleanupOldData(const std::chrono::system_clock::time_point& timestamp) = 0;
};

#define LOG_SQL_ERROR(QUERY) LOG_ERROR_MSG(fmt::format("Error in query: '{}'", (QUERY)))
