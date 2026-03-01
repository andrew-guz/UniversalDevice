#pragma once

#include <cstdint>
#include <vector>

#include "IQueryExecutor.hpp"
#include "TimeValuesController.hpp"
#include "UniversalValue.hpp"
#include "Uuid.hpp"

class UniversalValuesController final : public TimeValuesController<UniversalValue> {
public:
    UniversalValuesController(IQueryExecutor* queryExecutor);

    ~UniversalValuesController() = default;

    virtual bool Add(const Uuid& id, const UniversalValue& value) override;

    virtual std::vector<UniversalValue> Get(const Uuid& id, std::uint64_t seconds = 0) override;

protected:
    virtual void FillCache() override;
};
