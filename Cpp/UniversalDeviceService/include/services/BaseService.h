#ifndef _BASE_SERVICE_H_
#define _BASE_SERVICE_H_

#include <crow.h>

#include "IQueryExecutor.h"
#include "Logger.h"
#include "Message.h"

template<typename T>
class BaseService
{
protected:
    BaseService(IQueryExecutor* queryExecutor) :
        _queryExecutor(queryExecutor)
    {
        
    }

public:
    virtual ~BaseService() = default;

    static BaseService* Create(crow::SimpleApp& app, IQueryExecutor* queryExecutor)
    {
        T* t = new T(queryExecutor);
        t->Initialize(app);
        return t;
    }

protected:
    virtual void Initialize(crow::SimpleApp& app) = 0;

    Message GetMessageFromRequest(const crow::request& request)
    {
        auto body = request.body;
        try
        {           
            auto body_json = nlohmann::json::parse(body);
            LOG_INFO << body_json.dump() << std::endl;
            return Message::CreateFromJson(body_json);
        }
        catch(...)
        {
            LOG_ERROR << "Can't get message from request - " << body << std::endl;
        }
        return Message();        
    }

protected:
    IQueryExecutor* _queryExecutor = nullptr;
};

#endif //_BASE_SERVICE_H_
