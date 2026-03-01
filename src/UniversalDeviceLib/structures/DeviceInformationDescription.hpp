#pragma once

#include <cstdint>

#include "DeviceDescription.hpp"

struct DeviceInformationDescription final : public DeviceDescription {
    std::uint64_t _seconds = 0;

    virtual ~DeviceInformationDescription() = default;
};
