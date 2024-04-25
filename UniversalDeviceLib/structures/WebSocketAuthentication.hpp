#pragma once

#include "IJson.hpp"

struct WebSocketAuthentication final : IJson<WebSocketAuthentication> {
    std::string _authString;

    virtual nlohmann::json ToJson() const override { return { { "authString", _authString } }; }

    virtual void FromJson(const nlohmann::json& json) override { _authString = json.value("authString", ""); }
};
