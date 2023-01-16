#include "Service.h"

#include "Defines.h"

#include "Message.h"

std::string Service::Version()
{
    return VERSION;
}

int Service::Inform(const crow::request& request)
{
    if (request.method != crow::HTTPMethod::Post)
        return crow::BAD_REQUEST;
    return crow::OK;
}

int Service::Quit(crow::SimpleApp& app)
{
    app.stop();
    return crow::OK;
}
