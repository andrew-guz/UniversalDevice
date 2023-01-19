#ifndef _I_PROCESSOR_H_
#define _I_PROCESSOR_H_

#include <memory>
#include <vector>

#include "Message.h"

class IProcessor
{
public:
    IProcessor() = default;

    virtual ~IProcessor() = default;

    virtual void ProcessMessage(const Message& message) = 0;
};

typedef std::shared_ptr<IProcessor> Processor;
typedef std::vector<Processor> Processors;

#endif //_I_PROCESSOR_H_
