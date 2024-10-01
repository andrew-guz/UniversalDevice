#pragma once

#include <set>

#include "Uuid.hpp"

struct Scenario {
    Uuid _id;
    std::string _name;
    std::set<Uuid> _activateEvent;
    std::set<Uuid> _deactivateEvent;

    bool operator==(const Scenario&) const = default;
};
