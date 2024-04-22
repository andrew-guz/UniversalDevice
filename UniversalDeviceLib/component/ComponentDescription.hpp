#pragma once

#include <string>

#include "Constants.hpp"
#include "IJson.hpp"
#include "Uuid.hpp"

struct ComponentDescription : public IJson<ComponentDescription> {
    std::string _type;
    Uuid _id;

    virtual nlohmann::json ToJson() const override { return {{"type", _type}, {"id", _id.data()}}; }

    virtual void FromJson(const nlohmann::json& json) override {
        _type = json.value("type", "");
        _id = Uuid(json.value("id", ""));
    }
};
