#pragma once

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WVBoxLayout.h>

#include "Enums.hpp"
#include "Event.hpp"
#include "IEventEditorWidget.hpp"

class BaseEventEditor : public Wt::WContainerWidget, public IEventEditorWidget {
public:
    BaseEventEditor();

    virtual ~BaseEventEditor() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

protected:
    std::vector<ExtendedComponentDescription> FilteredDevices(DeviceType type);

    std::vector<ExtendedComponentDescription> FilteredDevices(const std::set<DeviceType>& types);

protected:
    std::vector<ExtendedComponentDescription> _devices;
    Wt::WVBoxLayout* _mainLayout;
    Wt::WLineEdit* _name;
    Wt::WCheckBox* _active;
};
