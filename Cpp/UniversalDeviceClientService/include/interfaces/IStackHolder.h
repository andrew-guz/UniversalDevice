#ifndef _STACK_HOLDER_H_
#define _STACK_HOLDER_H_

#include "Enums.h"

class IStackHolder
{
public:
    IStackHolder() = default;

    virtual ~IStackHolder() = default;

    virtual void SetWidget(StackWidgetType type, void* data) = 0;
};

#endif //_STACK_HOLDER_H_
