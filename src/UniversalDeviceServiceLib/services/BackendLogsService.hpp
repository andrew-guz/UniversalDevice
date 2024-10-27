#pragma once

#include "BaseService.hpp"

class BackendLogsService final : public BaseService {
protected:
    BackendLogsService(IQueryExecutor* queryExecutor);

public:
    virtual ~BackendLogsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetBackendLog() const;

    crow::response ClearBackendLog() const;

private:
    friend class BaseServiceExtension;
};
