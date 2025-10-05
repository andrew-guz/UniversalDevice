#pragma once

#include <memory>
#include <mutex>

#include <crow.h>

#include "CommandsController.hpp"
#include "CommandsService.hpp"
#include "DevicesController.hpp"
#include "DevicesService.hpp"
#include "IProcessor.hpp"
#include "IQueryExecutor.hpp"
#include "Message.hpp"
#include "Middleware.hpp"
#include "SettingsController.hpp"
#include "SettingsService.hpp"

struct Platform final {
protected:
    Platform(CrowApp& app, IQueryExecutor* queryExecutor);

public:
    static void Init(CrowApp& app, IQueryExecutor* queryExecutor);

    static Processors CreateProcessors(const Message& message, IQueryExecutor* queryExecutor);

private:
    static std::shared_ptr<Platform> s_instance;
    static std::once_flag s_instanceFlag;

    SettingsController _settingsController;
    CommandsController _commandsController;
    DevicesController _devicesController;

    SettingsService _settingsService;
    CommandsService _commandsService;
    DevicesService _deviceService;
};
