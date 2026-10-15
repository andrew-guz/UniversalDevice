#pragma once

#include <cstdint>
#include <string>

#include "Enums.hpp"

enum class StackWidgetType : std::uint8_t {
    Login,
    Devices,
    Events,
    Scenarios,
    Logs,
    Thermometer,
    Relay,
    MotionRelay,
    UniversalDevice,
};

class IStackHolder {
public:
    IStackHolder() = default;

    virtual ~IStackHolder() = default;

    virtual void SetWidget(StackWidgetType type, const std::string& data) = 0;

    virtual AccountType GetAccountType() const = 0;

    virtual void SetAccountType(AccountType accountType) = 0;

    bool CanEdit() const { return GetAccountType() == AccountType::User || GetAccountType() == AccountType::Admin; }

    bool IsAdmin() const { return GetAccountType() == AccountType::Admin; }
};
