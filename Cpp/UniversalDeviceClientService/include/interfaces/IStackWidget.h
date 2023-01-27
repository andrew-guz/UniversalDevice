#ifndef _I_STACK_WIDGET_H_
#define _I_STACK_WIDGET_H_

#include <string>

class IStackWidget
{
public:
    IStackWidget() = default;

    virtual ~IStackWidget() = default;

    virtual void Initialize(const std::string& data) = 0;
};

#endif //_I_STACK_WIDGET_H_
