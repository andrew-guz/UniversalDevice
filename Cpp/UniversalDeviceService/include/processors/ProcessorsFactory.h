#ifndef _PROCESSORS_FACTORY_H_
#define _PROCESSORS_FACTORY_H_

#include "IProcessor.h"
#include "Storage.h"

class ProcessorsFactory final
{
public:
    static Processors CreateProcessors(const Message& message, Storage& storage);
};

#endif //_PROCESSORS_FACTORY_H_
