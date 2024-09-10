#pragma once

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "ComponentDescription.hpp"
#include "Constants.hpp"
#include "Enums.hpp"
#include "Uuid.hpp"

struct Event {
    Uuid _id;
    std::string _name;
    bool _active = true;
    EventType _type;
    ComponentDescription _provider;
    ComponentDescription _receiver;
    nlohmann::json _command;

    Event(const EventType type = EventType::Undefined) :
        _type(type) {}

    virtual ~Event() = default;
};
