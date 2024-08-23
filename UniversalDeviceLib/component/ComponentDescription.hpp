#pragma once

#include <string>

#include "Uuid.hpp"

struct ComponentDescription {
    // This 99% is Device Type, like Relay or Thermometer, but can be TimerEvent. That why this was a string.
    // Also devices use this structure to send data
    std::string _type;
    Uuid _id;

    virtual ~ComponentDescription() = default;
};
