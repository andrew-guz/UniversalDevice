#pragma once

#include <optional>

#include "Cache.hpp"
#include "Command.hpp"
#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "Uuid.hpp"

class CommandsController final : public Controller {
public:
    CommandsController(IQueryExecutor* queryExecutor);

    ~CommandsController() = default;

    std::optional<Command> Get(const Uuid& deviceId) const;

    bool AddOrUpdate(const Uuid& deviceId, const Command& command);

    bool Remove(const Uuid& deviceId);

private:
    void FillCache();

private:
    mutable Cache<Uuid, Command> _cache;
};
