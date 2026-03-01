#pragma once

#include <chrono>

#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "MotionRelayValuesController.hpp"
#include "RelayValuesController.hpp"
#include "ThermometerValuesController.hpp"
#include "UniversalValuesController.hpp"

class CleanupController final : public Controller {
public:
    CleanupController(IQueryExecutor* queryExecutor,
                      ThermometerValuesController& thermometerValuesController,
                      RelayValuesController& relayValuesController,
                      MotionRelayValuesController& motionRelayValuesController,
                      UniversalValuesController& universalValuesController);

    ~CleanupController() = default;

    void Cleanup(const std::chrono::system_clock::time_point& timestamp);

private:
    ThermometerValuesController& _thermometerValuesController;
    RelayValuesController& _relayValuesController;
    MotionRelayValuesController& _motionRelayValuesController;
    UniversalValuesController& _universalValuesController;
};
