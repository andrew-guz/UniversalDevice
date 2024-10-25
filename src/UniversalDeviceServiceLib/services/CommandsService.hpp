#pragma once

#include "BaseService.hpp"

class CommandsService final : public BaseService {
protected:
    CommandsService(IQueryExecutor* queryExecutor);

public:
    virtual ~CommandsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetCommands(const std::string& idString) const;

    crow::response SetCommands(const crow::request& request, const std::string& idString);

private:
    friend class BaseServiceExtension;
};
