#include <iostream>
#include <crow.h>

#include "Settings.h"
#include "Service.h"

int main()
{
    auto settings = Settings::ReadSettings();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/version")(Service::Version);

    app
        .port(settings._port)
        .multithreaded()
        .run();

    return 0;
}
