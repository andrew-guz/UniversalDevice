#ifndef _PROCESSORS_FACTORY_H_
#define _PROCESSORS_FACTORY_H_

#include "IProcessor.h"
#include "IQueryExecutor.h"

class ProcessorsFactory final {
public:
    static Processors CreateProcessors(const Message& message, IQueryExecutor* queryExecutor);
};

#endif //_PROCESSORS_FACTORY_H_
