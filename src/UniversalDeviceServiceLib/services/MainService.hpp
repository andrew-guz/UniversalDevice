#pragma once

#include "BaseService.hpp"

class MainService final : public BaseService {
protected:
    MainService(IQueryExecutor* queryExecutor);

public:
    virtual ~MainService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

    crow::response GetVersion() const;

    crow::response SetLogLevel(const std::string& logLevel);

    crow::response Quit();

private:
    CrowApp* _application = nullptr;

    friend class ServiceExtension;
};
