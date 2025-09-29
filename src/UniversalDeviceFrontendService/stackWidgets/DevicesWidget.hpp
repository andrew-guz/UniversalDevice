#pragma once

#include <utility>
#include <vector>

#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>

#include "ApplicationSettings.hpp"
#include "BaseStackWidget.hpp"
#include "ExtendedComponentDescription.hpp"
#include "Scenario.hpp"

class DeviceButton;

class DevicesWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    DevicesWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

    virtual ~DevicesWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    void AddScenarioButton(Wt::WHBoxLayout* layout, const Scenario& scenario);

    void AddDeviceButton(Wt::WHBoxLayout* layout, const ExtendedComponentDescription& description);

    void OnSettings();

private:
    Wt::WVBoxLayout* _mainLayout = nullptr;
    std::vector<std::pair<Wt::WWidget*, Wt::WLayout*>> _widgets;
};
