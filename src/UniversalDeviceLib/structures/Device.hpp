#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "Enums.hpp"
#include "Uuid.hpp"

struct Device {
    Uuid _id;
    DeviceType _type;
    std::string _name;
    std::string _group;
    std::chrono::system_clock::time_point _timestamp;

    std::string GetDisplayString() const { return _name.size() ? _name : _id.data(); }

    std::string GetDisplayStringWithGroup() const {
        return _group.empty() ? GetDisplayString() : GetDisplayString() + std::string{ " (" } + _group + std::string{ ")" };
    }
};

typedef std::vector<Device> Devices;
