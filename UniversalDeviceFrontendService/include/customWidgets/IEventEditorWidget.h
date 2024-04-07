#ifndef _I_EVENT_EDITOR_WIDGET_H_
#define _I_EVENT_EDITOR_WIDGET_H_

#include <nlohmann/json.hpp>

#include "Event.h"
#include "ExtendedComponentDescription.h"

class IEventEditorWidget {
public:
    IEventEditorWidget() = default;

    virtual ~IEventEditorWidget() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) = 0;

    virtual void Cleanup() = 0;

    virtual void FillUi(const Event& event) = 0;

    virtual bool IsValid() const = 0;

    virtual void FillFromUi(Event& event) const = 0;
};

#endif //_I_EVENT_EDITOR_WIDGET_H_
