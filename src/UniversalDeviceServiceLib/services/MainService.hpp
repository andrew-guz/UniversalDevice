#pragma once

#include <string>

#include <crow/http_response.h>

#include "BaseService.hpp"
#include "Middleware.hpp"

class MainService final : public BaseService {
public:
    MainService(CrowApp& app);

    virtual ~MainService() = default;

protected:
    crow::response GetVersion() const;

    crow::response SetLogLevel(const std::string& logLevel);

    crow::response Quit();

private:
    CrowApp& _application;

    friend class ServiceExtension;
};
