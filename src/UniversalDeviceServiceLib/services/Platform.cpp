#include "Platform.hpp"

#include <crow.h>

#include "IQueryExecutor.hpp"
#include "Middleware.hpp"

Platform::Platform(CrowApp& app, IQueryExecutor* queryExecutor) :
    _settingsController(queryExecutor),
    _commandsController(queryExecutor),
    _settingsService(queryExecutor, _settingsController),
    _deviceService(queryExecutor, _settingsController) {}
