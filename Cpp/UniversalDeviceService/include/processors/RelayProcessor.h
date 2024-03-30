#ifndef _RELAY_PROCESSOR_H_
#define _RELAY_PROCESSOR_H_

#include "BaseProcessorWithQueryExecutor.h"

class RelayProcessor final : public BaseProcessorWithQueryExecutor {
public:
    RelayProcessor(IQueryExecutor* queryExecutor);

    virtual ~RelayProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};

#endif //_RELAY_PROCESSOR_H_
