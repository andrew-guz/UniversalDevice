#ifndef _PROCESSORS_FACTORY_H_
#define _PROCESSORS_FACTORY_H_

#include <crow.h>

#include "IProcessor.h"
#include "IQueryExecutor.h"

class ProcessorsFactory final
{
public:
    static Processors CreateProcessors(const Message& message, IQueryExecutor* queryExecutor);

    static Processors CreateProcessors(const Message& message, crow::websocket::connection* connection);
};

#endif //_PROCESSORS_FACTORY_H_
