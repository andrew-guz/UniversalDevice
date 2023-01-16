#include "Service.h"

#include "Defines.h"

std::string Service::Version()
{
    return VERSION;
}

int Service::Quit(crow::SimpleApp& app)
{
    app.stop();
    return crow::OK;
}
