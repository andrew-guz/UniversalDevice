#include "PostRequestHelper.h"

#include "Logger.h"
#include "RequestHelper.h"

void PostRequestHelper::DoInformRequest(const RequestAddress& requestAddress, const Message &message)
{
    try
    {
        auto message_json = message.ToJson();
        auto post_string = message_json.dump();

        LOG_INFO << "Posting " << post_string << "." << std::endl;
        
        RequestHelper::DoPostRequestWithNoAnswer(requestAddress, post_string);
    }
    catch(...)
    {
        LOG_ERROR << "Can't send message to " << requestAddress.BuildUrl() << "." << std::endl;
    }
    
}
