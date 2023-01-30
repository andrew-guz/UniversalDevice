#ifndef _REQUEST_HELPER_H_
#define _REQUEST_HELPER_H_

#include <string>

#include "RequestAddress.h"
#include "Message.h"

class RequestHelper final
{
public:
    static Message DoGetRequest(const RequestAddress& requestAddress);

    static int DoPostRequestWithNoAnswer(const RequestAddress& requestAddress, const Message& message);

    static Message DoPostRequestWithAnswer(const RequestAddress& requestAddress, const Message& message);

private:
    static int DoPostRequest(const RequestAddress& requestAddress, const Message& message, std::ostream* response);
};

#endif //_REQUEST_HELPER_H_
