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

    bool IsValidUser(const crow::request& request);

    bool IsValidUser(const std::string& authorization);

    void CallProcessorsNoResult(const std::chrono::system_clock::time_point& timestamp, const Message& message);

    nlohmann::json CallProcessorsJsonResult(const std::chrono::system_clock::time_point& timestamp, const Message& message);

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
