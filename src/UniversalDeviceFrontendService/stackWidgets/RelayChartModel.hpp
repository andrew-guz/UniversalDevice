#pragma once

#include <Wt/cpp17/any.hpp>

#include "BaseChartModel.hpp"
#include "ExtendedRelayCurrentState.hpp"

class RelayChartModel : public BaseChartModel<ExtendedRelayCurrentState> {
protected:
    virtual const Wt::cpp17::any GetValue(const int index) const override { return _data[index]._state; }
};
