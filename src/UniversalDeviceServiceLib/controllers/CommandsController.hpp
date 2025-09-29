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

    std::optional<Command> Get(const Uuid& id) const;

    bool AddOrUpdate(const Uuid& id, const Command& command);

    bool Remove(const Uuid& id);

private:
    mutable Cache<Uuid, Command> _cache;
};
