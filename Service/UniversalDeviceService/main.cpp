#include <iostream>
#include <crow.h>

#include "Settings.h"
#include "Service.h"

int main()
{
    auto settings = Settings::ReadSettings();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/version")(Service::Version);
    CROW_ROUTE(app, "/api/quit")([&](){ return Service::Quit(app); });

    app
        .port(settings._port)
        .multithreaded()
        .run();

    return 0;
}
