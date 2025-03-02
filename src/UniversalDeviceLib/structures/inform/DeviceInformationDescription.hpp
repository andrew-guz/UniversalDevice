#pragma once

#include <cstdint>

#include "ComponentDescription.hpp"

struct DeviceInformationDescription final : public ComponentDescription {
    std::uint64_t _seconds = 0;

    virtual ~DeviceInformationDescription() = default;
};
