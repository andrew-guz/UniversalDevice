#pragma once

#include <crow.h>

#include "DevicesService.hpp"
#include "IQueryExecutor.hpp"
#include "Middleware.hpp"
#include "SettingsController.hpp"
#include "SettingsService.hpp"

struct Platform final {
    Platform(CrowApp& app, IQueryExecutor* queryExecutor);

    SettingsController _settingsController;

    SettingsService _settingsService;
    DevicesService _deviceService;
};
