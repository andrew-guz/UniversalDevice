#ifndef _REQUEST_HELPER_H_
#define _REQUEST_HELPER_H_

#include <string>

#include "RequestAddress.h"
#include "Message.h"

class RequestHelper final
{
public:
    static nlohmann::json DoGetRequest(const RequestAddress& requestAddress);

    static int DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const nlohmann::json& json);

    static nlohmann::json DoPostRequestWithAnswer(const RequestAddress& requestAddress, const nlohmann::json& json);

private:
    static int DoPostRequest(const RequestAddress& requestAddress, const nlohmann::json& json, std::ostream* response);
};

#endif //_REQUEST_HELPER_H_
