#pragma once

#include <chrono>
#include <exception>
#include <string>
#include <string_view>
#include <utility>

#include <crow.h>
#include <crow/common.h>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Message.hpp"

class BaseService {
protected:
    BaseService() = default;

public:
    virtual ~BaseService() = default;

protected:
    void CallProcessorsNoResult(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json CallProcessorsJsonResult(const std::chrono::system_clock::time_point& timestamp, const Message& message);
};

class ServiceExtension final {
public:
    ServiceExtension() = delete;

    ~ServiceExtension() = default;

    static Message GetMessageFromRequest(const crow::request& request);

    static Message GetMessageFromWebSocketData(const std::string& data);

    template<typename ServiceType, typename... Args>
    static auto bind(ServiceType* service, crow::response (ServiceType::*func)(Args...)) {
        return [service, func](Args... args) { return (service->*func)(std::forward<Args>(args)...); };
    }

    template<typename ServiceType, typename... Args>
    static auto bind(ServiceType* service, crow::response (ServiceType::*func)(Args...) const) {
        return [service, func](Args... args) { return (service->*func)(std::forward<Args>(args)...); };
    }

    template<typename ServiceType, typename Object>
    static auto bindObject(ServiceType* service, crow::response (ServiceType::*func)(Object&), const std::string_view functionName) {
        return [service, func, functionName = std::move(functionName)](const crow::request& request) {
            try {
                auto bodyJson = nlohmann::json::parse(request.body);
                auto object = bodyJson.get<Object>();
                return (service->*func)(object);
            } catch (std::exception& ex) {
                LOG_ERROR_MSG(fmt::format("Error to execute {}: {}", functionName, ex.what()));
            } catch (...) {
                LOG_ERROR_MSG(fmt::format("Error to execute {}: unknown exception", functionName));
            }
            return crow::response(crow::BAD_REQUEST);
        };
    }

    template<typename ServiceType, typename Object>
    static auto bindObject(ServiceType* service, crow::response (ServiceType::*func)(const Object&), const std::string_view functionName) {
        return [service, func, functionName = std::move(functionName)](const crow::request& request) {
            try {
                auto bodyJson = nlohmann::json::parse(request.body);
                auto object = bodyJson.get<Object>();
                return (service->*func)(object);
            } catch (std::exception& ex) {
                LOG_ERROR_MSG(fmt::format("Error to execute {}: {}", functionName, ex.what()));
            } catch (...) {
                LOG_ERROR_MSG(fmt::format("Error to execute {}: unknown exception", functionName));
            }
            return crow::response(crow::BAD_REQUEST);
        };
    }

    template<typename ServiceType, typename Object>
    static auto
    bindObject(ServiceType* service, crow::response (ServiceType::*func)(const Object&, const std::string&), const std::string_view functionName) {
        return [service, func, functionName = std::move(functionName)](const crow::request& request) {
            try {
                auto bodyJson = nlohmann::json::parse(request.body);
                auto object = bodyJson.get<Object>();
                return (service->*func)(object, request.body);
            } catch (std::exception& ex) {
                LOG_ERROR_MSG(fmt::format("Error to execute {}: {}", functionName, ex.what()));
            } catch (...) {
                LOG_ERROR_MSG(fmt::format("Error to execute {}: unknown exception", functionName));
            }
            return crow::response(crow::BAD_REQUEST);
        };
    }
};
