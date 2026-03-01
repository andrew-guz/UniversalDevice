#pragma once

#include <string>

#include "Provider.hpp"
#include "Receiver.hpp"
#include "Uuid.hpp"

struct BaseEvent {
    Uuid _id;
    std::string _name;
    bool _active = true;
    Provider _provider;
    Receiver _receiver;
};
