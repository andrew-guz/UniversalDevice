#include "WebsocketsCache.hpp"

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

std::vector<crow::websocket::connection*> WebsocketsCache::ListWebSocketConnections() {
    std::lock_guard<std::mutex> lock(_webSocketConnectionsMutex);
    std::vector<crow::websocket::connection*> result;
    result.reserve(_webSocketConnections.size());
    for (auto iter = _webSocketConnections.begin(); iter != _webSocketConnections.end(); ++iter) {
        result.push_back(iter->second);
    }
    return result;
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
