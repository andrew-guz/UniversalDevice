#ifndef _MAIN_SERVICE_H_
#define _MAIN_SERVICE_H_

#include "BaseService.h"

class MainService final : public BaseService
{
protected:
    MainService(IQueryExecutor* queryExecutor);

public:
    virtual ~MainService() = default;

protected:
    virtual void Initialize(crow::SimpleApp& app) override;

    crow::response Version(const crow::request& request);

    crow::response Quit(const crow::request& request,crow::SimpleApp& app);

private:
    friend class BaseServiceExtension;
};

#endif //_MAIN_SERVICE_H_
