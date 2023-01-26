#include "RequestHelper.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "Logger.h"

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
    }
    catch(...)
    {
        LOG_ERROR << "Can't send request to " << requestAddress.BuildUrl() << std::endl;
    }
    
}
