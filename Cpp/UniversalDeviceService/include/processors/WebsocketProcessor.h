#ifndef _WEBSOCKET_PROCESSOR_H_
#define _WEBSOCKET_PROCESSOR_H_

#include "BaseProcessorWithQueryExecutor.h"

class WebSocketProcessor final : public BaseProcessorWithQueryExecutor
{
public:
    WebSocketProcessor(IQueryExecutor* queryExecutor);

    virtual ~WebSocketProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;
};

#endif //_WEBSOCKET_PROCESSOR_H_
