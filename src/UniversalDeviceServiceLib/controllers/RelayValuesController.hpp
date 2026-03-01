#pragma once

#include <cstdint>
#include <vector>

#include "IQueryExecutor.hpp"
#include "RelayValue.hpp"
#include "TimeValuesController.hpp"
#include "Uuid.hpp"

class RelayValuesController final : public TimeValuesController<RelayValue> {
public:
    RelayValuesController(IQueryExecutor* queryExecutor);

    ~RelayValuesController() = default;

    virtual bool Add(const Uuid& id, const RelayValue& value) override;

    virtual std::vector<RelayValue> Get(const Uuid& id, std::uint64_t seconds = 0) override;

protected:
    virtual void FillCache() override;
};
