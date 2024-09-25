#pragma once

#include "BaseService.hpp"

class MainService final : public BaseService {
protected:
    MainService(IQueryExecutor* queryExecutor);

public:
    virtual ~MainService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    friend class BaseServiceExtension;
};
