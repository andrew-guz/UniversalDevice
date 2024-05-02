#pragma once

#include <string>

#include "Constants.hpp"
#include "Uuid.hpp"

struct ComponentDescription {
    std::string _type;
    Uuid _id;

    virtual ~ComponentDescription() = default;
};
