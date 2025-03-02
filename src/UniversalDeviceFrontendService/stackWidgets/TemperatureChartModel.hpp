#pragma once

#include <Wt/cpp17/any.hpp>

#include "BaseChartModel.hpp"
#include "ExtendedThermometerCurrentValue.hpp"

class TemperatureChartModel : public BaseChartModel<ExtendedThermometerCurrentValue> {
protected:
    virtual const Wt::cpp17::any GetValue(const int index) const override { return _data[index]._value; }
};
