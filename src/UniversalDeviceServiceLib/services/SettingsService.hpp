#pragma once

#include <crow.h>

#include "Middleware.hpp"
#include "SettingsController.hpp"

class SettingsService final {
public:
    SettingsService(CrowApp& app, SettingsController& controller);

    ~SettingsService() = default;

private:
    crow::response GetSettings(const std::string& idString) const;

    crow::response SetSettings(const crow::request& request, const std::string& idString);

private:
    SettingsController& _controller;
};
