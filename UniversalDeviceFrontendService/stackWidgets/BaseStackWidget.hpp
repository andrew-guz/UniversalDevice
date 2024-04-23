#pragma once

#include "FrontendDefines.hpp"
#include "IStackHolder.hpp"
#include "IStackWidget.hpp"
#include "Settings.hpp"

class BaseStackWidget : public IStackWidget {
public:
    BaseStackWidget(IStackHolder* stackHolder, const Settings& settings) : _stackHolder(stackHolder), _settings(settings) {}

    virtual ~BaseStackWidget() = default;

protected:
    IStackHolder* _stackHolder;
    const Settings& _settings;
};
