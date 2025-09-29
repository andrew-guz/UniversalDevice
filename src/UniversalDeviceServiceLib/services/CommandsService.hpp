#pragma once

#include "BaseService.hpp"
#include "CommandsController.hpp"

class CommandsService final : public BaseService {
protected:
    CommandsService(IQueryExecutor* queryExecutor, CommandsController& controller);

public:
    virtual ~CommandsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetCommands(const std::string& idString) const;

    crow::response SetCommands(const crow::request& request, const std::string& idString);

private:
    CommandsController& _controller;
};
