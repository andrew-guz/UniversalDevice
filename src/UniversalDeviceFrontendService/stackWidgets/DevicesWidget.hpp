#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>

#include "BaseStackWidget.hpp"
#include "ExtendedComponentDescription.hpp"
#include "Scenario.hpp"
#include "Settings.hpp"

class DeviceButton;

class DevicesWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    DevicesWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~DevicesWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    DeviceButton* AddDeviceButton(Wt::WGridLayout* layout, const ExtendedComponentDescription& description, int& row, int& column);

    void AddScenarioButton(Wt::WGridLayout* layout, const Scenario& scenario, int& row, int& column);

private:
    Wt::WGridLayout* _mainLayout;
    std::map<Wt::WWidget*, Wt::WLayout*> _deviceWidgets;
};
