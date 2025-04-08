#pragma once

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WSpinBox.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

#include "DeviceComboBox.hpp"
#include "IEventEditorWidget.hpp"

class EventReceiverWidget final : public Wt::WContainerWidget, public IEventEditorWidget {
public:
    EventReceiverWidget();

    virtual ~EventReceiverWidget() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    void OnReceiverChanged();

private:
    Wt::WVBoxLayout* _mainLayout;
    DeviceComboBox* _receivers;
    Wt::WText* _brightnessText;
    Wt::WSpinBox* _brightness;
    Wt::WCheckBox* _relayState;
};
