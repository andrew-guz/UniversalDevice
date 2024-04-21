#ifndef _BASE_SERVICE_H_
#define _BASE_SERVICE_H_

#include <crow.h>

#include "IQueryExecutor.h"
#include "Logger.h"
#include "Message.h"
#include "Middleware.h"

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

    template<typename ServiceType, typename... Args>
    static auto bind(ServiceType* service, crow::response (ServiceType::*func)(Args...) const) {
        return [service, func](Args... args) { return (service->*func)(std::forward<Args>(args)...); };
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

#endif //_BASE_SERVICE_H_
