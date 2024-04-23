#pragma once

#include <string>

class IStackWidget {
public:
    IStackWidget() = default;

    virtual ~IStackWidget() = default;

    virtual void Initialize(const std::string& data) = 0;
};
