#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>
#include <string_view>

#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Message.hpp"
#include "Middleware.hpp"

class BaseService {
protected:
    BaseService(IQueryExecutor* queryExecutor);

public:
    virtual ~BaseService() = default;

protected:
    virtual void Initialize(CrowApp& app) = 0;

    void CallProcessorsNoResult(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json CallProcessorsJsonResult(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    template<typename ServiceType, typename... Args>
    static auto bind(ServiceType* service, crow::response (ServiceType::*func)(Args...)) {
        return [service, func](Args... args) { return (service->*func)(std::forward<Args>(args)...); };
    }

    template<typename ServiceType, typename Object>
    static auto bindObject(ServiceType* service, crow::response (ServiceType::*func)(const Object&), const std::string_view functionName) {
        return [service, func, functionName = std::move(functionName)](const crow::request& request) {
            try {
                auto bodyJson = nlohmann::json::parse(request.body);
                auto object = bodyJson.get<Object>();
                return (service->*func)(object);
            } catch (std::exception& ex) {
                LOG_ERROR << "Error to execute " << functionName << ": " << ex.what() << std::endl;
            } catch (...) {
                LOG_ERROR << "Error to execute " << functionName << ": unknown exception" << std::endl;
            }
            return crow::response(crow::BAD_REQUEST);
        };
    }

    template<typename ServiceType, typename Object>
    static auto bindObject(ServiceType* service, crow::response (ServiceType::*func)(const Object&, const std::string&),
                           const std::string_view functionName) {
        return [service, func, functionName = std::move(functionName)](const crow::request& request) {
            try {
                auto bodyJson = nlohmann::json::parse(request.body);
                auto object = bodyJson.get<Object>();
                return (service->*func)(object, request.body);
            } catch (std::exception& ex) {
                LOG_ERROR << "Error to execute " << functionName << ": " << ex.what() << std::endl;
            } catch (...) {
                LOG_ERROR << "Error to execute " << functionName << ": unknown exception" << std::endl;
            }
            return crow::response(crow::BAD_REQUEST);
        };
    }

protected:
    IQueryExecutor* _queryExecutor = nullptr;
};

class BaseServiceExtension final {
public:
    BaseServiceExtension() = delete;

    ~BaseServiceExtension() = default;

    template<typename ServiceType>
    static ServiceType* Create(CrowApp& app, IQueryExecutor* queryExecutor) {
        auto service = new ServiceType(queryExecutor);
        service->Initialize(app);
        return service;
    }

    static Message GetMessageFromRequest(const crow::request& request);

    static Message GetMessageFromWebSocketData(const std::string& data);
};
