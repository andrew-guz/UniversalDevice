#pragma once

#include <string>

#include "Uuid.hpp"

struct ComponentDescription {
    // TODOAGZ : this 99% is Device Typ like Relay but can be TimerEvent. That why this was a string....
    std::string _type;
    Uuid _id;

    virtual ~ComponentDescription() = default;
};
