#include <exception>

#include <crow.h>

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

        LOG_INFO << "Starting Device service..." << std::endl;

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

        LOG_INFO << "Stopping Device service" << std::endl;
    } catch (const std::exception& ex) {
        LOG_ERROR << "Exception caught: '" << ex.what() << "'" << std::endl;
    } catch (...) {
        LOG_ERROR << "Unknown exception caught" << std::endl;
    }

    return 0;
}
