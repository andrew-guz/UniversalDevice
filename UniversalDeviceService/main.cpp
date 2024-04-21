#include <crow.h>

#include "ClientService.h"
#include "DeviceService.h"
#include "Logger.h"
#include "MainService.h"
#include "Middleware.h"
#include "Settings.h"
#include "Storage.h"

int main() {
    Logger::SetLogLevel(LogLevel::INFO);

    auto settings = Settings::ReadSettings();

    Storage storage;

    CrowApp app;

    BaseServiceExtension::Create<MainService>(app, &storage);
    BaseServiceExtension::Create<DeviceService>(app, &storage);
    BaseServiceExtension::Create<ClientService>(app, &storage);

    app.ssl_file(PathHelper::FullFilePath("./ssl/backend.crt"), PathHelper::FullFilePath("./ssl/backend.key")).port(settings._port).multithreaded().run();

    return 0;
}
