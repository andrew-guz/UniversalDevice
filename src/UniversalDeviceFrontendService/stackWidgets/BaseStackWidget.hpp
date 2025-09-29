#pragma once

#include "ApplicationSettings.hpp"
#include "FrontendDefines.hpp"
#include "IStackHolder.hpp"
#include "IStackWidget.hpp"

class BaseStackWidget : public IStackWidget {
public:
    BaseStackWidget(IStackHolder* stackHolder, const ApplicationSettings& settings) :
        _stackHolder(stackHolder),
        _settings(settings) {}

    virtual ~BaseStackWidget() = default;

protected:
    IStackHolder* _stackHolder;
    const ApplicationSettings& _settings;
};
