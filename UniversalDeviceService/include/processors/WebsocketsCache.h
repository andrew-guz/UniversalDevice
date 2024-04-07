#ifndef _WEBSOCKET_CAHCE_H_
#define _WEBSOCKET_CAHCE_H_

#include <crow.h>
#include <map>
#include <mutex>

#include "Singleton.h"
#include "Uuid.h"

class WebsocketsCache final : public Singleton<WebsocketsCache> {
public:
    void AddWebSocketConnection(const Uuid& id, crow::websocket::connection& connection);

    crow::websocket::connection* GetWebSocketConnection(const Uuid& id);

    void DeleteWebSocketConnection(crow::websocket::connection& connection);

private:
    std::mutex _webSocketConnectionsMutex;
    std::map<Uuid, crow::websocket::connection*> _webSocketConnections;
};

#endif //_WEBSOCKET_CAHCE_H_