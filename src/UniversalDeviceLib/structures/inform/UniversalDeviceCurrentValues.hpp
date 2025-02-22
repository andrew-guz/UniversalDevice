#pragma once

#include <map>
#include <string>

#include "UniversalData.hpp"

struct UniversalDeviceCurrentValues {
    std::map<std::string, UniversalData> _values;
};
