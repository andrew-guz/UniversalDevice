#pragma once

#include <string>
#include <string_view>

#include "Message.hpp"
#include "RequestAddress.hpp"

class RequestHelper final {
public:
    static nlohmann::json DoGetRequest(const RequestAddress& requestAddress, std::string_view login);

    static int DoPostRequest(const RequestAddress& requestAddress, std::string_view login, const nlohmann::json& json);

    static nlohmann::json DoPostRequestWithAnswer(const RequestAddress& requestAddress, std::string_view login, const nlohmann::json& json);

    static int DoPutRequest(const RequestAddress& requestAddress, std::string_view login, const nlohmann::json& json);

    static int DoPatchRequest(const RequestAddress& requestAddress, std::string_view login, const nlohmann::json& json);

    static int DoDeleteRequest(const RequestAddress& requestAddress, std::string_view login, const nlohmann::json& json = {});

private:
    static int DoRequest(const std::string& method,
                         const RequestAddress& requestAddress,
                         std::string_view login,
                         const nlohmann::json& json,
                         std::ostream* responseStream);
};
