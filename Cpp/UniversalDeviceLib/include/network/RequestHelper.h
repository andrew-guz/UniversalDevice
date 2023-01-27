#ifndef _REQUEST_HELPER_H_
#define _REQUEST_HELPER_H_

#include <string>

#include "RequestAddress.h"
#include "Message.h"

class RequestHelper final
{
public:
    static std::string DoGetRequest(const RequestAddress& requestAddress);

    static void DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const std::string& post_string);
};

#endif //_REQUEST_HELPER_H_
