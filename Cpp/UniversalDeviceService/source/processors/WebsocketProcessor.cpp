#include "WebsocketProcessor.h"

WebSocketProcessor::WebSocketProcessor(crow::websocket::connection* connection) :
    _connection(connection)
{

}

nlohmann::json WebSocketProcessor::ProcessMessage(const std::chrono::system_clock::time_point &timestamp, const Message &message)
{
    return nlohmann::json();
}
