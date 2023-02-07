#include "RequestHelper.h"

#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include "Logger.h"

nlohmann::json RequestHelper::DoGetRequest(const RequestAddress& requestAddress)
{
    try
    {
        std::string url = requestAddress.BuildUrl();
        LOG_DEBUG << "GET " << url << "." << std::endl;

        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(url)); 
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
            auto bodyJson = nlohmann::json::parse(body);
            LOG_DEBUG << "GET result - " << bodyJson.dump() << std::endl;
            return bodyJson;
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
    return {};
}

int RequestHelper::DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const nlohmann::json& json)
{
    return DoPostRequest(requestAddress, json, nullptr);
}

nlohmann::json RequestHelper::DoPostRequestWithAnswer(const RequestAddress& requestAddress, const nlohmann::json& json)
{
    std::ostringstream response;
    if (DoPostRequest(requestAddress, json, &response) == 200)
    {
        auto body = response.str();
        LOG_DEBUG << "POST result - " << body << std::endl;
        try
        {
            auto bodyJson = nlohmann::json::parse(body);
            return bodyJson;
        }
        catch(...)
        {
            LOG_ERROR << "Failed to parse POST result as JSON." << std::endl;
        }        
    }
    return {};
}

int RequestHelper::DoPostRequest(const RequestAddress &requestAddress, const nlohmann::json& json, std::ostream* response)
{
    try
    {
        auto postString = json.dump();

        LOG_DEBUG << "POST " << requestAddress.BuildUrl() << " " <<  postString << "." << std::endl;

        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(requestAddress.BuildUrl())); 
        request.setOpt(new cURLpp::options::Verbose(true)); 
        
        std::list<std::string> header; 
        header.push_back("Content-Type: application/json");
        request.setOpt(new curlpp::options::HttpHeader(header)); 
    
        request.setOpt(new curlpp::options::PostFields(postString));
        request.setOpt(new curlpp::options::PostFieldSize(postString.size()));

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
