#include "RequestHelper.h"

#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>

#include "Logger.h"

std::string RequestHelper::DoGetRequest(const RequestAddress& requestAddress)
{
    try
    {
        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        std::stringstream result;

        request.setOpt(new cURLpp::options::Url(requestAddress.BuildUrl())); 
        request.setOpt(new cURLpp::options::Verbose(true)); 
        request.setOpt(new cURLpp::options::WriteStream(&result)); 
        
        request.perform();

        auto returnCode = curlpp::infos::ResponseCode::get(request);
        if (returnCode == 200)
            return result.str();
        else
            LOG_ERROR << "POST request failed : " << returnCode << "." << std::endl;       
    }
    catch(...)
    {
        LOG_ERROR << "GET request failed (" << requestAddress.BuildUrl() << ")." << std::endl;
    }

    return {};
}

void RequestHelper::DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const std::string& post_string)
{
    try
    {
        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(requestAddress.BuildUrl())); 
        request.setOpt(new cURLpp::options::Verbose(true)); 
        
        std::list<std::string> header; 
        header.push_back("Content-Type: application/json");
        request.setOpt(new curlpp::options::HttpHeader(header)); 
    
        request.setOpt(new curlpp::options::PostFields(post_string));
        request.setOpt(new curlpp::options::PostFieldSize(post_string.size()));

        request.perform();

        auto returnCode = curlpp::infos::ResponseCode::get(request);
        if (returnCode != 200)
            LOG_ERROR << "POST request failed : " << returnCode << "." << std::endl;
    }
    catch(...)
    {
        LOG_ERROR << "POST request failed (" << requestAddress.BuildUrl() << ")." << std::endl;
    }    
}
