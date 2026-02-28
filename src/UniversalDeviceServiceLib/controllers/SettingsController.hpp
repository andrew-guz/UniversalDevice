#pragma once

#include <optional>

#include "Cache.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "Settings.hpp"
#include "Uuid.hpp"

class SettingsController final : public Controller {
public:
    SettingsController(IQueryExecutor* queryExecutor);

    ~SettingsController() = default;

    std::optional<Settings> Get(const Uuid& deviceId) const;

    bool AddOrUpdate(const Uuid& deviceId, const Settings& settings);

    bool Remove(const Uuid& deviceId);

private:
    void FillCache();

private:
    mutable Cache<Uuid, Settings> _cache;
};
