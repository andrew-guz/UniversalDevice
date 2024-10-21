#pragma once

#include "BaseChartModel.hpp"
#include "ExtendedRelayCurrentState.hpp"

class RelayChartModel : public BaseChartModel<ExtendedRelayCurrentState> {};
