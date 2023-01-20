#include "PostRequestHelper.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

void PostRequestHelper::DoInformRequest(const RequestAddress& requestAddress, const Message &message)
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
    
        auto message_json = message.ToJson();
        auto post_string = message_json.dump();

        request.setOpt(new curlpp::options::PostFields(post_string));
        request.setOpt(new curlpp::options::PostFieldSize(post_string.size()));

        request.perform();
    }
    catch(...)
    {
        
    }
    
}
