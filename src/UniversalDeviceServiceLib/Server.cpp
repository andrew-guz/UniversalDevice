#include "Server.hpp"

#include <crow.h>
#include <fmt/format.h>

#include "AccountManagerInitializer.hpp"
#include "BackendLogsService.hpp"
#include "CommandsService.hpp"
#include "DeviceWebsocketsService.hpp"
#include "DevicesService.hpp"
#include "EventsService.hpp"
#include "FirmwareService.hpp"
#include "Logger.hpp"
#include "MainService.hpp"
#include "Middleware.hpp"
#include "PathHelper.hpp"
#include "ScenariosService.hpp"
#include "Settings.hpp"
#include "SettingsService.hpp"
#include "Storage.hpp"
#include "TimerService.hpp"

int Server::run() {
    try {
        auto settings = Settings::ReadSettings();
        if (!settings._logPath.empty())
            PathHelper::SetCustomLogPath(settings._logPath);

        Logger::SetLogLevel(LogLevel::INFO);

        LOG_INFO_MSG("Starting Device service...");

        AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializer>(PathHelper::FullFilePath(settings._authPath)));

        Storage storage(PathHelper::FullFilePath(settings._dbPath));

        CrowApp app;

        BaseServiceExtension::Create<MainService>(app, &storage);
        BaseServiceExtension::Create<SettingsService>(app, &storage);
        BaseServiceExtension::Create<CommandsService>(app, &storage);
        BaseServiceExtension::Create<EventsService>(app, &storage);
        BaseServiceExtension::Create<ScenariosService>(app, &storage);
        BaseServiceExtension::Create<BackendLogsService>(app, &storage);
        BaseServiceExtension::Create<DevicesService>(app, &storage);
        BaseServiceExtension::Create<DeviceWebsocketsService>(app, &storage);
        BaseServiceExtension::Create<TimerService>(app, &storage);
        BaseServiceExtension::Create<FirmwareService>(app, &storage);

        app.ssl_file(PathHelper::FullFilePath(settings._certificatePath).native(), PathHelper::FullFilePath(settings._keyPath).native())
            .port(settings._port)
            .multithreaded()
            .run();

        LOG_INFO_MSG("Stopping Device service");
    } catch (const std::exception& ex) {
        LOG_ERROR_MSG(fmt::format("Exception caught: '{}'", ex.what()));
    } catch (...) {
        LOG_ERROR_MSG("Unknown exception caught");
    }

    return 0;
}
