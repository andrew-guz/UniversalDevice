#include "RequestHelper.h"

#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include "Logger.h"

Message RequestHelper::DoGetRequest(const RequestAddress& requestAddress)
{
    try
    {
        LOG_INFO << "GET " << requestAddress.BuildUrl() << "." << std::endl;

        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(requestAddress.BuildUrl())); 
        request.setOpt(new cURLpp::options::Verbose(true)); 
        

        std::ostringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        auto returnCode = curlpp::infos::ResponseCode::get(request);
        if (returnCode != 200)
            LOG_ERROR << "GET request failed : " << returnCode << "." << std::endl;

        auto body = response.str();
        try
        {
            auto body_json = nlohmann::json::parse(body);
            LOG_INFO << body_json.dump() << std::endl;
            return Message::CreateFromJson(body_json);
        }
        catch(...)
        {
            LOG_ERROR << "Invalid response " << body << "." << std::endl;
        }    
    }
    catch(...)
    {
        LOG_ERROR << "GET request failed (" << requestAddress.BuildUrl() << ")." << std::endl;
    }
    return Message();
}

int RequestHelper::DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const Message& message)
{
    return DoPostRequest(requestAddress, message, nullptr);
}

Message RequestHelper::DoPostRequestWithAnswer(const RequestAddress& requestAddress, const Message& message)
{
    std::ostringstream response;
    if (DoPostRequest(requestAddress, message, &response) == 200)
    {
        auto body = response.str();
        try
        {
            auto body_json = nlohmann::json::parse(body);
            LOG_INFO << body_json.dump() << std::endl;
            return Message::CreateFromJson(body_json);
        }
        catch(...)
        {
            LOG_ERROR << "Invalid response " << body << "." << std::endl;
        }        
    }
    return Message();
}

int RequestHelper::DoPostRequest(const RequestAddress &requestAddress, const Message& message, std::ostream* response)
{
    try
    {
        auto message_json = message.ToJson();
        auto post_string = message_json.dump();

        LOG_INFO << "POST " << requestAddress.BuildUrl() << " data " <<  post_string << "." << std::endl;

        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(requestAddress.BuildUrl())); 
        request.setOpt(new cURLpp::options::Verbose(true)); 
        
        std::list<std::string> header; 
        header.push_back("Content-Type: application/json");
        request.setOpt(new curlpp::options::HttpHeader(header)); 
    
        request.setOpt(new curlpp::options::PostFields(post_string));
        request.setOpt(new curlpp::options::PostFieldSize(post_string.size()));

        if (response)
            request.setOpt(new curlpp::options::WriteStream(response));

        request.perform();

        auto returnCode = curlpp::infos::ResponseCode::get(request);
        if (returnCode != 200)
            LOG_ERROR << "POST request failed : " << returnCode << "." << std::endl;

        return returnCode;
    }
    catch(...)
    {
        LOG_ERROR << "POST request failed (" << requestAddress.BuildUrl() << ")." << std::endl;
    }
    return 400;
}
