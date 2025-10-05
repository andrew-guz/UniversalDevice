#pragma once

#include <chrono>

#include "Enums.hpp"
#include "Uuid.hpp"

struct Device {
    Uuid _id;
    DeviceType _type;
    std::string _name;
    std::string _group;
    std::chrono::system_clock::time_point _timestamp;
};
