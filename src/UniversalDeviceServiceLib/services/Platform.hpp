#pragma once

#include <memory>
#include <mutex>

#include <crow.h>

#include "BackendLogsService.hpp"
#include "CleanupController.hpp"
#include "CommandsController.hpp"
#include "CommandsService.hpp"
#include "DeviceWebsocketsService.hpp"
#include "DevicesController.hpp"
#include "DevicesService.hpp"
#include "EventsController.hpp"
#include "EventsService.hpp"
#include "IProcessor.hpp"
#include "IQueryExecutor.hpp"
#include "MainService.hpp"
#include "Message.hpp"
#include "Middleware.hpp"
#include "MotionRelayValuesController.hpp"
#include "RelayValuesController.hpp"
#include "ScenariosController.hpp"
#include "ScenariosService.hpp"
#include "SettingsController.hpp"
#include "SettingsService.hpp"
#include "ThermometerValuesController.hpp"
#include "TimerService.hpp"
#include "UniversalValuesController.hpp"

struct Platform final {
protected:
    Platform(CrowApp& app, IQueryExecutor* queryExecutor);

public:
    static void Init(CrowApp& app, IQueryExecutor* queryExecutor);

    static Processors CreateProcessors(const Message& message);

private:
    static std::shared_ptr<Platform> s_instance;
    static std::once_flag s_instanceFlag;

    SettingsController _settingsController;
    CommandsController _commandsController;
    DevicesController _devicesController;
    EventsController _eventsController;
    ScenariosController _scenariosController;
    ThermometerValuesController _thermometerValuesController;
    RelayValuesController _relayValuesController;
    MotionRelayValuesController _motionRelayValuesController;
    UniversalValuesController _universalValuesController;
    CleanupController _cleanupController;

    MainService _mainService;
    SettingsService _settingsService;
    CommandsService _commandsService;
    DevicesService _deviceService;
    DeviceWebsocketsService _deviceWebsocketsService;
    EventsService _eventsService;
    ScenariosService _scenariosService;
    BackendLogsService _backendLogsService;
    TimerService _timerService;
};
