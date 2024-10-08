#include "RequestHelper.hpp"

#include <sstream>

#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <fmt/format.h>

#include "AccountManager.hpp"
#include "Logger.hpp"

nlohmann::json RequestHelper::DoGetRequest(const RequestAddress& requestAddress, const std::string_view login) {
    try {
        std::string url = requestAddress.BuildUrl();
        LOG_DEBUG_MSG(fmt::format("GET {}", url));

        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(url));
        request.setOpt(new cURLpp::options::Verbose(true));

        request.setOpt(curlpp::options::SslVerifyPeer(false));
        request.setOpt(curlpp::options::SslVerifyHost(false));

        request.setOpt(new curlpp::options::UserPwd(AccountManager::Instance()->GetAuthString(login)));

        std::ostringstream response;
        request.setOpt(new curlpp::options::WriteStream(&response));

        request.perform();

        auto returnCode = curlpp::infos::ResponseCode::get(request);
        if (returnCode != 200)
            LOG_ERROR_MSG(fmt::format("GET request failed : {}", returnCode));

        auto body = response.str();
        if (body.empty())
            return {};
        try {
            auto bodyJson = nlohmann::json::parse(body);
            LOG_DEBUG_MSG(fmt::format("GET result - {}", bodyJson.dump()));
            return bodyJson;
        } catch (...) {
            LOG_ERROR_MSG(fmt::format("Invalid response {}", body));
        }
    } catch (...) {
        LOG_ERROR_MSG(fmt::format("GET request failed ({})", requestAddress.BuildUrl()));
    }
    return {};
}

int RequestHelper::DoPostRequest(const RequestAddress& requestAddress, const std::string_view login, const nlohmann::json& json) {
    return DoRequest("POST", requestAddress, login, json, nullptr);
}

nlohmann::json
RequestHelper::DoPostRequestWithAnswer(const RequestAddress& requestAddress, const std::string_view login, const nlohmann::json& json) {
    std::ostringstream response;
    if (DoRequest("POST", requestAddress, login, json, &response) == 200) {
        auto body = response.str();
        LOG_DEBUG_MSG(fmt::format("POST result - {}", body));
        try {
            auto bodyJson = nlohmann::json::parse(body);
            return bodyJson;
        } catch (...) {
            LOG_ERROR_MSG("Failed to parse POST result as JSON");
        }
    }
    return {};
}

int RequestHelper::DoPutRequest(const RequestAddress& requestAddress, const std::string_view login, const nlohmann::json& json) {
    return DoRequest("PUT", requestAddress, login, json, nullptr);
}

int RequestHelper::DoDeleteRequest(const RequestAddress& requestAddress, const std::string_view login, const nlohmann::json& json) {
    return DoRequest("DELETE", requestAddress, login, json, nullptr);
}

int RequestHelper::DoRequest(const std::string& method,
                             const RequestAddress& requestAddress,
                             const std::string_view login,
                             const nlohmann::json& json,
                             std::ostream* response) {
    if (method != "POST" && method != "PUT" && method != "DELETE")
        throw new std::bad_function_call();
    try {
        std::string url = requestAddress.BuildUrl();
        auto sendingString = json.dump();

        LOG_DEBUG_MSG(fmt::format("{} {} {}", method, url, sendingString));

        cURLpp::Cleanup cleaner;
        cURLpp::Easy request;

        request.setOpt(new cURLpp::options::Url(url));
        request.setOpt(new cURLpp::options::Verbose(true));

        request.setOpt(curlpp::options::SslVerifyPeer(false));
        request.setOpt(curlpp::options::SslVerifyHost(false));

        std::list<std::string> header;
        header.push_back("Content-Type: application/json");
        request.setOpt(new curlpp::options::HttpHeader(header));

        request.setOpt(new curlpp::options::UserPwd(AccountManager::Instance()->GetAuthString(login)));

        if (method == "PUT" || method == "DELETE")
            request.setOpt(new curlpp::options::CustomRequest(method));

        request.setOpt(new curlpp::options::PostFields(sendingString));
        request.setOpt(new curlpp::options::PostFieldSize(sendingString.size()));

        if (response)
            request.setOpt(new curlpp::options::WriteStream(response));

        request.perform();

        auto returnCode = curlpp::infos::ResponseCode::get(request);
        if (returnCode != 200)
            LOG_ERROR_MSG(fmt::format("{} request failed: {}", method, returnCode));

        return returnCode;
    } catch (...) {
        LOG_ERROR_MSG(fmt::format("{} request failed ({})", method, requestAddress.BuildUrl()));
    }
    return 400;
}
