#include <iostream>
#include <crow.h>

#include "Settings.h"
#include "Storage.h"
#include "MainService.h"
#include "DeviceService.h"
#include "ClientService.h"

int main()
{
    auto settings = Settings::ReadSettings();

    Storage storage;

    crow::SimpleApp app;

    MainService::Create(app, &storage);
    DeviceService::Create(app, &storage);
    ClientService::Create(app, &storage);

    app
        .port(settings._port)
        .multithreaded()
        .run();

    return 0;
}
