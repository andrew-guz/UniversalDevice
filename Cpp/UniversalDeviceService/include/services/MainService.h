#ifndef _MAIN_SERVICE_H_
#define _MAIN_SERVICE_H_

#include "BaseService.h"

class MainService final : public BaseService<MainService>
{
protected:
    MainService(IQueryExecutor* queryExecutor);

public:
    virtual ~MainService() = default;

protected:
    virtual void Initialize(crow::SimpleApp& app) override;

    std::string Version();

    int Quit(crow::SimpleApp& app);

private:
    friend class BaseService<MainService>;
};

#endif //_MAIN_SERVICE_H_
