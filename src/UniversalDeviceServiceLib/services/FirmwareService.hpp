#pragma once

#include "BaseService.hpp"
#include "Settings.hpp"

class FirmwareService final : public BaseService {
protected:
    FirmwareService(IQueryExecutor* queryExecutor, const Settings& settings);

public:
    virtual ~FirmwareService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response UploadFirmware(const crow::request& request, const std::string& idString) const;

private:
    const Settings& _settings;

private:
    friend class BaseServiceExtension;
};
