#include <exception>

#include <crow.h>
#include <fmt/format.h>

#include "ClientService.hpp"
#include "DeviceService.hpp"
#include "Logger.hpp"
#include "MainService.hpp"
#include "Middleware.hpp"
#include "Settings.hpp"
#include "Storage.hpp"

int main() {
    try {
        Logger::SetLogLevel(LogLevel::INFO);

        LOG_INFO_MSG("Starting Device service...");

        auto settings = Settings::ReadSettings();

        Storage storage;

        CrowApp app;

        BaseServiceExtension::Create<MainService>(app, &storage);
        BaseServiceExtension::Create<DeviceService>(app, &storage);
        BaseServiceExtension::Create<ClientService>(app, &storage);

        app.ssl_file(PathHelper::FullFilePath("./ssl/backend.crt"), PathHelper::FullFilePath("./ssl/backend.key"))
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
