#pragma once

#include <set>

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WVBoxLayout.h>

#include "Device.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "IEventEditorWidget.hpp"

class BaseEventEditor : public Wt::WContainerWidget, public IEventEditorWidget {
public:
    BaseEventEditor();

    virtual ~BaseEventEditor() = default;

    virtual void SetDevices(const Devices& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

protected:
    Devices FilteredDevices(DeviceType type);

    Devices FilteredDevices(const std::set<DeviceType>& types);

protected:
    Devices _devices;
    Wt::WVBoxLayout* _mainLayout;
    Wt::WLineEdit* _name;
    Wt::WCheckBox* _active;
};
