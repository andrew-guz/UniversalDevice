#pragma once

#include <cstdint>
#include <vector>

#include "IQueryExecutor.hpp"
#include "MotionRelayValue.hpp"
#include "TimeValuesController.hpp"
#include "Uuid.hpp"

class MotionRelayValuesController final : public TimeValuesController<MotionRelayValue> {
public:
    MotionRelayValuesController(IQueryExecutor* queryExecutor);

    ~MotionRelayValuesController() = default;

    virtual bool Add(const Uuid& id, const MotionRelayValue& value) override;

    virtual std::vector<MotionRelayValue> Get(const Uuid& id, std::uint64_t seconds = 0) override;

protected:
    virtual void FillCache() override;
};
