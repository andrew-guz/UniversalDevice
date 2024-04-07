#ifndef _WEBSOCKET_AUTHENTICATION_H_
#define _WEBSOCKET_AUTHENTICATION_H_

#include "IJson.h"

struct WebSocketAuthentication final : IJson<WebSocketAuthentication> {
    std::string _authString;

    virtual nlohmann::json ToJson() const override { return {{"authString", _authString}}; }

    virtual void FromJson(const nlohmann::json& json) override { _authString = json.value("authString", ""); }
};

#endif //_WEBSOCKET_AUTHENTICATION_H_
