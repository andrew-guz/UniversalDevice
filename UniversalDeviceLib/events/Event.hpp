#pragma once

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "ComponentDescription.hpp"
#include "Constants.hpp"
#include "Uuid.hpp"

struct Event {
    Uuid _id;
    std::string _name;
    bool _active = true;
    std::string _type;
    ComponentDescription _provider;
    ComponentDescription _receiver;
    nlohmann::json _command;

    Event(const std::string_view type = Constants::EventTypeUndefined) : _type(type) {}

    virtual ~Event() = default;
};
