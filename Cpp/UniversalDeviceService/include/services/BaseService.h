#ifndef _BASE_SERVICE_H_
#define _BASE_SERVICE_H_

#include <crow.h>

#include "IQueryExecutor.h"
#include "Logger.h"
#include "Message.h"

class BaseService
{
protected:
    BaseService(IQueryExecutor* queryExecutor) :
        _queryExecutor(queryExecutor)
    {
        
    }

public:
    virtual ~BaseService() = default;
    

protected:
    virtual void Initialize(crow::SimpleApp& app) = 0;

protected:
    IQueryExecutor* _queryExecutor = nullptr;
};

class BaseServiceExtension final
{
public:
    BaseServiceExtension() = delete;

    ~BaseServiceExtension() = default;

    template<typename T>
    static BaseService* Create(crow::SimpleApp& app, IQueryExecutor* queryExecutor)
    {
        auto t = new T(queryExecutor);
        t->Initialize(app);
        return t;
    }

    static Message GetMessageFromRequest(const crow::request& request)
    {
        auto body = request.body;
        try
        {           
            auto bodyJson = nlohmann::json::parse(body);
            LOG_INFO << bodyJson.dump() << std::endl;
            return JsonExtension::CreateFromJson<Message>(bodyJson);
        }
        catch(...)
        {
            LOG_ERROR << "Can't get message from request - " << body << std::endl;
        }
        return Message();        
    }

};

#endif //_BASE_SERVICE_H_
