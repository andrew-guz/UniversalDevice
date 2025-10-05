#pragma once

#include <crow.h>

#include "CommandsController.hpp"
#include "Middleware.hpp"

class CommandsService final {
public:
    CommandsService(CrowApp& app, CommandsController& controller);

    virtual ~CommandsService() = default;

private:
    crow::response GetCommands(const std::string& idString) const;

    crow::response SetCommands(const crow::request& request, const std::string& idString);

private:
    CommandsController& _controller;
};
