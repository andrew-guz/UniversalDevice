#pragma once

#include "BaseChartModel.hpp"
#include "ExtendedThermometerCurrentValue.hpp"

class TemperatureChartModel : public BaseChartModel<ExtendedThermometerCurrentValue, float> {
protected:
    virtual const float GetValue(const int index) const override { return _data[index]._value; }
};
