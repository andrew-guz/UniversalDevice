#pragma once

#include <set>
#include <tuple>

#include "Uuid.hpp"

struct Scenario {
    Uuid _id;
    std::string _name;
    std::set<Uuid> _activateEvent;
    std::set<Uuid> _deactivateEvent;

    bool operator==(const Scenario& other) const {
        return std::tie(_id, _name, _activateEvent, _deactivateEvent) == std::tie(other._id, other._name, other._activateEvent, other._deactivateEvent);
    }
};
