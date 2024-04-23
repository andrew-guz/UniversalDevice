#pragma once

#include "Enums.hpp"

class IStackHolder {
public:
    IStackHolder() = default;

    virtual ~IStackHolder() = default;

    virtual void SetWidget(StackWidgetType type, const std::string& data) = 0;
};
