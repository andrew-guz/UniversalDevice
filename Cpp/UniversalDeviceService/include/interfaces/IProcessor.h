#ifndef _I_PROCESSOR_H_
#define _I_PROCESSOR_H_

#include <chrono>
#include <memory>
#include <vector>

#include "Message.h"

class IProcessor
{
public:
    IProcessor() = default;

    virtual ~IProcessor() = default;

    virtual void ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) = 0;
};

typedef std::shared_ptr<IProcessor> Processor;
typedef std::vector<Processor> Processors;

#endif //_I_PROCESSOR_H_
