#pragma once

#include <nlohmann/json.hpp>

#include "Event.hpp"
#include "ExtendedComponentDescription.hpp"

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
