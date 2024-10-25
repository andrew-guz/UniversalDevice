#pragma once

#include "BaseService.hpp"

class EventsService final : public BaseService {
protected:
    EventsService(IQueryExecutor* queryExecutor);

public:
    virtual ~EventsService() = default;

protected:
    virtual void Initialize(CrowApp& app) override;

private:
    crow::response GetEvents() const;

    crow::response AddEvent(const Event& event, const std::string& eventString);

    crow::response UpdateEvent(const Event& event, const std::string& eventString);

    crow::response DeleteEvent(const Event& event);

private:
    friend class BaseServiceExtension;
};
