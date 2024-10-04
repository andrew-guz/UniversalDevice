#pragma once

#include "BaseService.hpp"
#include "Event.hpp"

class ClientService final : public BaseService {
protected:
    ClientService(IQueryExecutor* queryExecutor);

public:
    virtual ~ClientService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response ListDevices() const;

    crow::response GetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field) const;

    crow::response SetDeviceProperty(const crow::request& request, const std::string& idString, const std::string& field, bool canBeEmpty);

    crow::response GetDeviceInfo(const crow::request& request);

    crow::response GetEvents() const;

    crow::response AddEvent(const Event& event, const std::string& eventString);

    crow::response UpdateEvent(const Event& event, const std::string& eventString);

    crow::response DeleteEvent(const Event& event);

    crow::response GetScenarios() const;

    crow::response GetBackendLog() const;

private:
    friend class BaseServiceExtension;
};
