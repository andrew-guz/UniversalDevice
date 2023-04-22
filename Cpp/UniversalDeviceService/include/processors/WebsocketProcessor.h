#ifndef _WEBSOCKET_PROCESSOR_H_
#define _WEBSOCKET_PROCESSOR_H_

#include <crow.h>

#include "IProcessor.h"

class WebSocketProcessor final : public IProcessor
{
public:
    WebSocketProcessor(crow::websocket::connection* connection);

    virtual ~WebSocketProcessor() = default;

    virtual nlohmann::json ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) override;

private:
    crow::websocket::connection* _connection;
};

#endif //_WEBSOCKET_PROCESSOR_H_
