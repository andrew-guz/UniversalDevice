#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "IQueryExecutor.hpp"

class Storage : public IQueryExecutor {
protected:
    Storage() = default;

public:
    virtual ~Storage() = default;

    virtual std::vector<std::string> GetAllTables() const override;

    virtual std::vector<std::string> GetDeviceRelatedTables() const override;

    virtual std::vector<std::string> GetDataTables() const override;

protected:
    void InitializeDb();

private:
    std::mutex _mutex;
};
