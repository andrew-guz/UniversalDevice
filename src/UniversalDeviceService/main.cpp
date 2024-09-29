#include <cstring>
#include <exception>
#include <filesystem>

#include <crow.h>
#include <fmt/format.h>

#include "ClientService.hpp"
#include "DeviceService.hpp"
#include "Logger.hpp"
#include "MainService.hpp"
#include "Middleware.hpp"
#include "PathHelper.hpp"
#include "Settings.hpp"
#include "Storage.hpp"
#include "Version.hpp"

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        std::cout << fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH) << std::endl;
        return 0;
    }

    if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        std::filesystem::path databasePath = argv[2];
        PathHelper::SetAppDbPath(databasePath);
    }

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
