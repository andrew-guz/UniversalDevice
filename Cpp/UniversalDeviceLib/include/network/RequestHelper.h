#ifndef _REQUEST_HELPER_H_
#define _REQUEST_HELPER_H_

#include "RequestAddress.h"
#include "Message.h"

class RequestHelper final
{
public:
    static void DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const std::string& post_string);
};

#endif //_REQUEST_HELPER_H_
