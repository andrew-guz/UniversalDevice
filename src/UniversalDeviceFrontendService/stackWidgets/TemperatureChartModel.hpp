#pragma once

#include <Wt/cpp17/any.hpp>

#include "BaseChartModel.hpp"
#include "ThermometerValue.hpp"

class TemperatureChartModel : public BaseChartModel<ThermometerValue> {
protected:
    virtual const Wt::cpp17::any GetValue(const int index) const override { return _data[index]._value; }
};
