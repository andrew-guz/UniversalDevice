#ifndef _MOTION_RELAY_PROCESSOR_H_
#define _MOTION_RELAY_PROCESSOR_H_

#include "BaseProcessorWithQueryExecutor.h"

class MotionRelayProcessor final : public BaseProcessorWithQueryExecutor {
public:
    MotionRelayProcessor(IQueryExecutor* queryExecutor);

    virtual ~MotionRelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};

#endif //_MOTION_RELAY_PROCESSOR_H_
