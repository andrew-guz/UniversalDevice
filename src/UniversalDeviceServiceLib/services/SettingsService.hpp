#pragma once

#include "BaseService.hpp"

class SettingsService final : public BaseService {
protected:
    SettingsService(IQueryExecutor* queryExecutor);

public:
    virtual ~SettingsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetSettings(const std::string& idString) const;

    crow::response SetSettings(const crow::request& request, const std::string& idString);

private:
    friend class BaseServiceExtension;
};
