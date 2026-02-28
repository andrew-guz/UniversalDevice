#pragma once

#include <crow/http_response.h>

#include "BaseService.hpp"
#include "Middleware.hpp"

class BackendLogsService final : public BaseService {
public:
    BackendLogsService(CrowApp& app);

    virtual ~BackendLogsService() = default;

private:
    crow::response GetBackendLog() const;

    crow::response ClearBackendLog() const;

private:
    friend class ServiceExtension;
};
