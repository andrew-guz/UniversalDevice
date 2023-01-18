#include <iostream>
#include <crow.h>

#include "Settings.h"
#include "Service.h"

int main()
{
    auto settings = Settings::ReadSettings();

    Service service;

    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/version")([&](){ return service.Version(); });
    CROW_ROUTE(app, "/api/inform").methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return service.Inform(request); });
    CROW_ROUTE(app, "/api/quit")([&](){ return service.Quit(app); });

    app
        .port(settings._port)
        .multithreaded()
        .run();

    return 0;
}
