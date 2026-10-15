#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <vector>

#include "IQueryExecutor.hpp"

class Storage final : public IQueryExecutor {
private:
    struct Query final {
        explicit Query(sqlite3_stmt* statement) :
            _statement(statement) {}

        ~Query() { sqlite3_finalize(_statement); }

        Query(const Query& other) = delete;

        Query(Query&& other) = delete;

        Query& operator=(const Query& other) = delete;

        Query& operator=(Query&& other) = delete;

        sqlite3_stmt* _statement = nullptr;
    };

public:
    Storage(const std::filesystem::path& dbPath);

    virtual ~Storage();

    virtual std::vector<std::string> GetAllTables() const override;

    virtual std::vector<std::string> GetDeviceRelatedTables() const override;

    virtual std::vector<std::string> GetDataTables() const override;

    virtual void CleanupOldData(const std::chrono::system_clock::time_point& timestamp) override;

    /************************************************************************* */
protected:
    [[nodiscard]] virtual std::int64_t NewInsertImpl(std::string_view query, const DatabaseValues& databaseValues) override;

    [[nodiscard]] virtual bool
    NewSelectImpl(std::vector<std::vector<std::string>>& data, std::string_view query, const DatabaseValues& databaseValues) override;

    [[nodiscard]] virtual bool NewUpdateImpl(std::string_view query, const DatabaseValues& databaseValues) override;

    [[nodiscard]] virtual bool NewDeleteImpl(std::string_view query, const DatabaseValues& databaseValues) override;

    virtual bool Begin() override;

    virtual bool Rollback() override;

    virtual bool Commit() override;

    bool ExecuteRaw(std::string_view query, const DatabaseValues& databaseValues, std::string_view errorMsg);

private:
    std::unique_ptr<Storage::Query> CreateQuery(std::string_view query);

    int BindQuery(sqlite3_stmt* statement, const DatabaseValues& databaseValues);

    void InitializeDb();

private:
    sqlite3* _connection = nullptr;
    std::filesystem::path _dbPath;
    std::mutex _mutex;
};
