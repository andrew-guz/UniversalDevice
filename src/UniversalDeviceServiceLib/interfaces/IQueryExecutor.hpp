#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <fmt/format.h>

#include "Logger.hpp"

using DatabaseValue = std::variant<std::string, std::int64_t, float>;

using DatabaseValues = std::vector<DatabaseValue>;

template<typename T>
DatabaseValue ToDatabaseValue(const T& value) = delete;

template<>
inline DatabaseValue ToDatabaseValue(const std::string& value) {
    return DatabaseValue{ value };
}

template<>
inline DatabaseValue ToDatabaseValue(const bool& value) {
    return DatabaseValue{ std::int64_t{ value ? 1 : 0 } };
}

template<>
inline DatabaseValue ToDatabaseValue(const int& value) {
    return DatabaseValue{ static_cast<std::int64_t>(value) };
}

template<>
inline DatabaseValue ToDatabaseValue(const std::int64_t& value) {
    return DatabaseValue{ value };
}

template<>
inline DatabaseValue ToDatabaseValue(const float& value) {
    return DatabaseValue{ value };
}

class Transaction;

class IQueryExecutor {
public:
    IQueryExecutor() = default;

    virtual ~IQueryExecutor() = default;

    template<typename... Args>
    [[nodiscard]] std::int64_t NewInsert(const std::string_view query, const Args&... args) {
        return NewInsertImpl(query, { ToDatabaseValue(args)... });
    }

    template<typename... Args>
    [[nodiscard]] bool NewSelect(std::vector<std::vector<std::string>>& data, const std::string_view query, const Args&... args) {
        return NewSelectImpl(data, query, { ToDatabaseValue(args)... });
    }

    template<typename... Args>
    [[nodiscard]] bool NewUpdate(const std::string_view query, const Args&... args) {
        return NewUpdateImpl(query, { ToDatabaseValue(args)... });
    }

    template<typename... Args>
    [[nodiscard]] bool NewDelete(const std::string_view query, const Args&... args) {
        return NewDeleteImpl(query, { ToDatabaseValue(args)... });
    }

    virtual std::vector<std::string> GetAllTables() const = 0;

    virtual std::vector<std::string> GetDeviceRelatedTables() const = 0;

    virtual std::vector<std::string> GetDataTables() const = 0;

    virtual void CleanupOldData(const std::chrono::system_clock::time_point& timestamp) = 0;

protected:
    [[nodiscard]] virtual std::int64_t NewInsertImpl(std::string_view query, const DatabaseValues& databaseValues) = 0;

    [[nodiscard]] virtual bool
    NewSelectImpl(std::vector<std::vector<std::string>>& data, std::string_view query, const DatabaseValues& databaseValues) = 0;

    [[nodiscard]] virtual bool NewUpdateImpl(std::string_view query, const DatabaseValues& databaseValues) = 0;

    [[nodiscard]] virtual bool NewDeleteImpl(std::string_view query, const DatabaseValues& databaseValues) = 0;

    virtual bool Begin() = 0;

    virtual bool Rollback() = 0;

    virtual bool Commit() = 0;

private:
    friend class Transaction;
};

class Transaction final {
public:
    explicit Transaction(IQueryExecutor* executor) :
        _executor(executor),
        _active(executor->Begin()) {}

    ~Transaction() {
        if (_active)
            _executor->Rollback();
    }

    Transaction(const Transaction&) = delete;

    Transaction& operator=(const Transaction&) = delete;

    [[nodiscard]] bool IsActive() const { return _active; }

    [[nodiscard]] bool Commit() {
        if (!_active)
            return false;

        _active = false;
        return _executor->Commit();
    }

private:
    IQueryExecutor* _executor;
    bool _active;
};

#define LOG_SQL_ERROR(QUERY) LOG_ERROR_MSG(fmt::format("Error in query: '{}'", (QUERY)))
