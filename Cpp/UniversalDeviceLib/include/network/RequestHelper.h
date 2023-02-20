#ifndef _REQUEST_HELPER_H_
#define _REQUEST_HELPER_H_

#include <string>

#include "RequestAddress.h"
#include "Message.h"

class RequestHelper final
{
public:
    static nlohmann::json DoGetRequest(const RequestAddress& requestAddress, const std::string& login);

    static int DoPostRequest(const RequestAddress& requestAddress, const std::string& login, const nlohmann::json& json);

    static nlohmann::json DoPostRequestWithAnswer(const RequestAddress& requestAddress, const std::string& login, const nlohmann::json& json);

    static int DoPutRequest(const RequestAddress& requestAddress, const std::string& login, const nlohmann::json& json);

    static int DoDeleteRequest(const RequestAddress& requestAddress, const std::string& login, const nlohmann::json& json);

private:
    static int DoRequest(const std::string& method, const RequestAddress& requestAddress, const std::string& login, const nlohmann::json& json, std::ostream* response);
};

#endif //_REQUEST_HELPER_H_
