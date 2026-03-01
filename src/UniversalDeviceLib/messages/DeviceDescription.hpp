#pragma once

#include "Enums.hpp"
#include "Uuid.hpp"

struct DeviceDescription {
    DeviceType _type = DeviceType::Undefined;
    Uuid _id;
};
