#pragma once

#include <crow.h>

#include "CommandsController.hpp"
#include "DevicesService.hpp"
#include "IQueryExecutor.hpp"
#include "Middleware.hpp"
#include "SettingsController.hpp"
#include "SettingsService.hpp"

struct Platform final {
    Platform(CrowApp& app, IQueryExecutor* queryExecutor);

    SettingsController _settingsController;
    CommandsController _commandsController;

    SettingsService _settingsService;
    DevicesService _deviceService;
};
