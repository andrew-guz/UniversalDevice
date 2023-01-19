#include "Service.h"

#include "Defines.h"
#include "Serializer.h"
#include "ProcessorsFactory.h"

Service::Service()
{

}

Service::~Service()
{

}

std::string Service::Version()
{
    return VERSION;
}

int Service::Inform(const crow::request& request)
{
    auto body = request.body;
    Message message;
    try
    {
        auto body_json = nlohmann::json::parse(body);
        std::cout << "Inform:  " << body_json.dump() << std::endl;
        message = Serializer<Message>::ToObject(body_json);
        auto processors = ProcessorsFactory::CreateProcessors(message, _storage);
        for (auto& processor : processors)
            processor->ProcessMessage(message);
    }
    catch(...)
    {
        return crow::BAD_REQUEST;
    }    
    return crow::OK;
}

int Service::Quit(crow::SimpleApp& app)
{
    app.stop();
    return crow::OK;
}
