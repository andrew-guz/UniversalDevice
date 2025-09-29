#pragma once

#include "BaseService.hpp"
#include "SettingsController.hpp"

class SettingsService final : public BaseService {
public:
    SettingsService(IQueryExecutor* queryExecutor, SettingsController& controller);

    virtual ~SettingsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetSettings(const std::string& idString) const;

    crow::response SetSettings(const crow::request& request, const std::string& idString);

private:
    SettingsController& _controller;
};
