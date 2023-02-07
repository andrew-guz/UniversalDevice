#include <iostream>
#include <crow.h>

#include "Logger.h"
#include "Settings.h"
#include "Storage.h"
#include "MainService.h"
#include "DeviceService.h"
#include "ClientService.h"

int main()
{
    Logger::SetLogLevel(LogLevel::INFO);

    auto settings = Settings::ReadSettings();

    Storage storage;

    crow::SimpleApp app;

    BaseServiceExtension::Create<MainService>(app, &storage);
    BaseServiceExtension::Create<DeviceService>(app, &storage);
    BaseServiceExtension::Create<ClientService>(app, &storage);

    app
        .port(settings._port)
        .multithreaded()
        .run();

    return 0;
}
