#ifndef _POST_REQUEST_HELPER_H_
#define _POST_REQUEST_HELPER_H_

#include "RequestAddress.h"
#include "Message.h"

class PostRequestHelper
{
public:
    static void DoInformRequest(const RequestAddress& requestAddress, const Message& message);
};

#endif //_POST_REQUEST_HELPER_H_
