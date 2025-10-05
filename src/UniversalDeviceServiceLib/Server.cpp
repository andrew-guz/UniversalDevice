#include "Server.hpp"

#include <crow.h>
#include <fmt/format.h>

#include "AccountManagerInitializer.hpp"
#include "ApplicationSettings.hpp"
#include "BackendLogsService.hpp"
#include "DeviceWebsocketsService.hpp"
#include "EventsService.hpp"
#include "Logger.hpp"
#include "MainService.hpp"
#include "Middleware.hpp"
#include "PathHelper.hpp"
#include "Platform.hpp"
#include "ScenariosService.hpp"
#include "Storage.hpp"
#include "TimerService.hpp"

int Server::run() {
    try {
        auto settings = ApplicationSettings::ReadSettings();
        if (!settings._logPath.empty())
            PathHelper::SetCustomLogPath(settings._logPath);

        Logger::SetLogLevel(settings._logLevel);

        LOG_INFO_MSG("Starting Device service...");

        AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializer>(PathHelper::FullFilePath(settings._authPath)));

        Storage storage(PathHelper::FullFilePath(settings._dbPath));

        CrowApp app;

        Platform::Init(app, &storage);

        ServiceExtension::Create<MainService>(app, &storage);
        ServiceExtension::Create<EventsService>(app, &storage);
        ServiceExtension::Create<ScenariosService>(app, &storage);
        ServiceExtension::Create<BackendLogsService>(app, &storage);
        ServiceExtension::Create<DeviceWebsocketsService>(app, &storage);
        ServiceExtension::Create<TimerService>(app, &storage);

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
