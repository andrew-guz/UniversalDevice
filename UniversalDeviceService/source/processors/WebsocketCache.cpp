#include "WebsocketsCache.h"

void WebsocketsCache::AddWebSocketConnection(const Uuid& id, crow::websocket::connection& connection) {
    std::lock_guard<std::mutex> lock(_webSocketConnectionsMutex);
    auto iter = std::find_if(_webSocketConnections.begin(), _webSocketConnections.end(), [&id](const auto& p) { return p.first == id; });
    if (iter != _webSocketConnections.end() && iter->second != &connection)
        _webSocketConnections.erase(iter);
    _webSocketConnections.insert(std::make_pair(id, &connection));
}

crow::websocket::connection* WebsocketsCache::GetWebSocketConnection(const Uuid& id) {
    std::lock_guard<std::mutex> lock(_webSocketConnectionsMutex);
    auto iter = std::find_if(_webSocketConnections.begin(), _webSocketConnections.end(), [&id](const auto& p) { return p.first == id; });
    if (iter != _webSocketConnections.end())
        return iter->second;
    return nullptr;
}

void WebsocketsCache::DeleteWebSocketConnection(crow::websocket::connection& connection) {
    std::lock_guard<std::mutex> lock(_webSocketConnectionsMutex);
    for (auto iter = _webSocketConnections.begin(); iter != _webSocketConnections.end();) {
        if (iter->second == &connection)
            iter = _webSocketConnections.erase(iter);
        else
            ++iter;
    }
}
