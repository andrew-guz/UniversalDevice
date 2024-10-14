#pragma once

#include <cstdint>

enum class StackWidgetType : std::uint8_t {
    Login,
    Devices,
    Events,
    Scenarios,
    Logs,
    Thermometer,
    Relay,
    MotionRelay,
};

class IStackHolder {
public:
    IStackHolder() = default;

    virtual ~IStackHolder() = default;

    virtual void SetWidget(StackWidgetType type, const std::string& data) = 0;
};
