#pragma once

#include "IJson.hpp"
#include "MessageHeader.hpp"

struct Message final : public IJson<Message> {
    MessageHeader _header;
    nlohmann::json _data;

    virtual nlohmann::json ToJson() const override { return {{"header", _header.ToJson()}, {"data", _data}}; }

    virtual void FromJson(const nlohmann::json& json) override {
        _header.FromJson(json["header"]);
        _data = json["data"];
    }
};
