#include "CleanupController.hpp"

#include <chrono>

#include "Controller.hpp"
#include "IQueryExecutor.hpp"
#include "MotionRelayValuesController.hpp"
#include "RelayValuesController.hpp"
#include "ThermometerValuesController.hpp"
#include "UniversalValuesController.hpp"
#include <bits/chrono.h>

namespace {
    constexpr std::chrono::weeks oldDataDelta = std::chrono::weeks{ 2 };
}

CleanupController::CleanupController(IQueryExecutor* queryExecutor,
                                     ThermometerValuesController& thermometerValuesController,
                                     RelayValuesController& relayValuesController,
                                     MotionRelayValuesController& motionRelayValuesController,
                                     UniversalValuesController& universalValuesController) :
    Controller(queryExecutor),
    _thermometerValuesController(thermometerValuesController),
    _relayValuesController(relayValuesController),
    _motionRelayValuesController(motionRelayValuesController),
    _universalValuesController(universalValuesController) {}

void CleanupController::Cleanup(const std::chrono::system_clock::time_point& timestamp) {
    const std::chrono::system_clock::time_point cleanupTimestamp = timestamp - oldDataDelta;
    _queryExecutor->CleanupOldData(cleanupTimestamp);
    _thermometerValuesController.Cleanup(cleanupTimestamp);
    _relayValuesController.Cleanup(cleanupTimestamp);
    _motionRelayValuesController.Cleanup(cleanupTimestamp);
    _universalValuesController.Cleanup(cleanupTimestamp);
}
