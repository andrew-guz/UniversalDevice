#include "RequestHelper.hpp"

#include <sstream>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <ixwebsocket/IXHttp.h>
#include <ixwebsocket/IXHttpClient.h>
#include <ixwebsocket/IXSocketTLSOptions.h>

#include "AccountManager.hpp"
#include "Base64Helper.hpp"
#include "Logger.hpp"

nlohmann::json RequestHelper::DoGetRequest(const RequestAddress& requestAddress, const std::string_view login) {
    try {
        const std::string url = requestAddress.BuildUrl();
        LOG_DEBUG_MSG(fmt::format("GET {}", url));

        ix::HttpClient client;
        ix::SocketTLSOptions options;
        options.caFile = "NONE";
        options.disable_hostname_validation = true;
        client.setTLSOptions(options);
        ix::HttpRequestArgsPtr args = client.createRequest(url, "GET");
        args->extraHeaders.emplace("Authorization",
                                   fmt::format("Basic {}", Base64Helper::ToBase64(AccountManager::Instance()->GetAuthString(login))));
        const ix::HttpResponsePtr response = client.request(url, "GET", {}, args);

        if (response->statusCode != 200)
            LOG_ERROR_MSG(fmt::format("GET request failed : {}", response->statusCode));

        const std::string& body = response->body;
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

int RequestHelper::DoPatchRequest(const RequestAddress& requestAddress, std::string_view login, const nlohmann::json& json) {
    return DoRequest("PATCH", requestAddress, login, json, nullptr);
}

int RequestHelper::DoDeleteRequest(const RequestAddress& requestAddress, const std::string_view login, const nlohmann::json& json) {
    return DoRequest("DELETE", requestAddress, login, json, nullptr);
}

std::pair<int, std::string> RequestHelper::DoGetOutsizeRequest(const std::string& url) {
    ix::HttpClient client;
    ix::SocketTLSOptions options;
    options.caFile = "NONE";
    options.disable_hostname_validation = true;
    client.setTLSOptions(options);
    ix::HttpRequestArgsPtr args = client.createRequest(url, ix::HttpClient::kGet);
    const ix::HttpResponsePtr response = client.request(url, "GET", {}, args);

    if (response->statusCode != 200) {
        LOG_ERROR_MSG(fmt::format("GET request failed: {} {}", response->statusCode, response->errorMsg));
        return std::make_pair(response->statusCode, response->errorMsg);
    }

    return std::make_pair(response->statusCode, response->body);
}

int RequestHelper::DoRequest(const std::string& method,
                             const RequestAddress& requestAddress,
                             const std::string_view login,
                             const nlohmann::json& json,
                             std::ostream* responseStream) {
    if (method != "POST" && method != "PUT" && method != "PATCH" && method != "DELETE")
        throw new std::bad_function_call();
    try {
        std::string url = requestAddress.BuildUrl();
        auto sendingString = json.dump();

        LOG_DEBUG_MSG(fmt::format("{} {} {}", method, url, sendingString));

        ix::HttpClient client;
        ix::SocketTLSOptions options;
        options.caFile = "NONE";
        options.disable_hostname_validation = true;
        client.setTLSOptions(options);
        ix::HttpRequestArgsPtr args = client.createRequest(url, ix::HttpClient::kGet);
        args->extraHeaders.emplace("Authorization",
                                   fmt::format("Basic {}", Base64Helper::ToBase64(AccountManager::Instance()->GetAuthString(login))));
        args->extraHeaders.emplace("Content-Type", "application/json");
        if (method == "DELETE")
            client.setForceBody(true);
        const ix::HttpResponsePtr response = client.request(url, method, sendingString, args);

        if (response->statusCode != 200)
            LOG_ERROR_MSG(fmt::format("{} request failed: {} {}", method, response->statusCode, response->errorMsg));

        if (responseStream)
            *responseStream << response->body;

        return response->statusCode;
    } catch (...) {
        LOG_ERROR_MSG(fmt::format("{} request failed ({})", method, requestAddress.BuildUrl()));
    }
    return 400;
}
