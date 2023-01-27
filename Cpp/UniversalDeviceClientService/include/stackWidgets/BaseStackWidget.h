#ifndef _BASE_STACK_WIDGET_H_
#define _BASE_STACK_WIDGET_H_

#include "IStackHolder.h"
#include "Settings.h"

class BaseStackWidget
{
public:
    BaseStackWidget(IStackHolder* stackHolder, const Settings& settings) :
        _stackHolder(stackHolder),
        _settings(settings)
    {}

    virtual ~BaseStackWidget() = default;

protected:
    IStackHolder*   _stackHolder;
    const Settings& _settings;
};

#endif //_BASE_STACK_WIDGET_H_
