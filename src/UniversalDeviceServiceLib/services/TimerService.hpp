#pragma once

#include "BaseService.hpp"

class TimerService final : public BaseService {
protected:
    TimerService(IQueryExecutor* queryExecutor);

public:
    virtual ~TimerService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    void TimerFunction();

private:
    friend class ServiceExtension;
};
