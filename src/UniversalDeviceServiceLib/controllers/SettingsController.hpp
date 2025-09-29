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

    std::optional<Settings> Get(const Uuid& id) const;

    bool AddOrUpdate(const Uuid& id, const Settings& settings);

    bool Remove(const Uuid& id);

private:
    mutable Cache<Uuid, Settings> _cache;
};
