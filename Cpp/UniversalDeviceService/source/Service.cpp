#include "Service.h"

#include "Defines.h"
#include "Serializer.h"

std::string Service::Version()
{
    return VERSION;
}

int Service::Inform(const crow::request& request)
{
    if (request.method != crow::HTTPMethod::Post)
        return crow::BAD_REQUEST;
    auto body = request.body;
    auto body_json = nlohmann::json::parse(body);
    auto message = Serializer<Message>::ToObject(body_json);
    return crow::OK;
}

int Service::Quit(crow::SimpleApp& app)
{
    app.stop();
    return crow::OK;
}
