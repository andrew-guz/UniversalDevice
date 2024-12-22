#pragma once

#include "BaseService.hpp"

class FirmwareService final : public BaseService {
protected:
    FirmwareService(IQueryExecutor* queryExecutor);

public:
    virtual ~FirmwareService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response UploadFirmware(const crow::request& request, const std::string& idString) const;

    crow::response ProvideFirmware(const std::string& idString) const;

private:
    friend class BaseServiceExtension;
};
