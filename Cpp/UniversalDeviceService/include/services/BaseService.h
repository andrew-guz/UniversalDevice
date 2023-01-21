#ifndef _BASE_SERVICE_H_
#define _BASE_SERVICE_H_

#include <crow.h>

#include "IQueryExecutor.h"

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

protected:
    IQueryExecutor* _queryExecutor = nullptr;
};

#endif //_BASE_SERVICE_H_
