#ifndef _THERMOMETER_PROCESSOR_H_
#define _THERMOMETER_PROCESSOR_H_

#include "BaseProcessorWithQueryExecutor.h"

class ThermometerProcessor final : public BaseProcessorWithQueryExecutor
{
public:
    ThermometerProcessor(IQueryExecutor* queryExecutor);

    virtual ~ThermometerProcessor() = default;

    virtual void ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};

#endif //_THERMOMETER_PROCESSOR_H_
