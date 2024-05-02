#pragma once

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>

#include "DeviceComboBox.hpp"
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
    std::vector<ExtendedComponentDescription> FilteredDevices(std::string_view type);

    std::vector<ExtendedComponentDescription> FilteredDevices(const std::set<std::string_view>& types);

protected:
    std::vector<ExtendedComponentDescription> _devices;
    Wt::WGridLayout* _mainLayout;
    Wt::WLineEdit* _name;
    Wt::WCheckBox* _active;
};
