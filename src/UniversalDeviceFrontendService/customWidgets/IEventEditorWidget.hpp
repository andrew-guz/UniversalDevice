#pragma once

#include "Device.hpp"
#include "Event.hpp"

class IEventEditorWidget {
public:
    IEventEditorWidget() = default;

    virtual ~IEventEditorWidget() = default;

    virtual void SetDevices(const Devices& devices) = 0;

    virtual void Cleanup() = 0;

    virtual void FillUi(const Event& event) = 0;

    virtual bool IsValid() const = 0;

    virtual void FillFromUi(Event& event) const = 0;
};
