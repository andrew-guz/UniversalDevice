#ifndef _THERMOMETER_PROCESSOR_H_
#define _THERMOMETER_PROCESSOR_H_

#include "BaseProcessorUsingStorage.h"

class ThermometerProcessor final : public BaseProcessorUsingStorage
{
public:
    ThermometerProcessor(Storage& storage);

    virtual ~ThermometerProcessor() = default;

    virtual void ProcessMessage(const Message& message) override;
};

#endif //_THERMOMETER_PROCESSOR_H_
