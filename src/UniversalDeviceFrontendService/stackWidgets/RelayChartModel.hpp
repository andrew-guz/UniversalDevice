#pragma once

#include "BaseChartModel.hpp"
#include "ExtendedRelayCurrentState.hpp"

class RelayChartModel : public BaseChartModel<ExtendedRelayCurrentState, int> {
protected:
    virtual const int GetValue(const int index) const override { return _data[index]._state; }
};
