#pragma once

#include "ComponentDescription.hpp"

struct DeviceInformationDescription final : public ComponentDescription {
    uint64_t _seconds = 0;

    virtual ~DeviceInformationDescription() = default;
};
