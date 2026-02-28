#pragma once

#include <cstdint>
#include <vector>

#include "DevicesController.hpp"
#include "IQueryExecutor.hpp"
#include "ThermometerValue.hpp"
#include "TimeValuesController.hpp"
#include "Uuid.hpp"

class ThermometerValuesController final : public TimeValuesController<ThermometerValue> {
public:
    ThermometerValuesController(IQueryExecutor* queryExecutor, DevicesController& devicesController);

    ~ThermometerValuesController() = default;

    virtual bool Add(const Uuid& id, const ThermometerValue& value) override;

    virtual std::vector<ThermometerValue> Get(const Uuid& id, std::uint64_t seconds = 0) override;

protected:
    virtual void FillCache() override;

private:
    DevicesController& _devicesController;
};
