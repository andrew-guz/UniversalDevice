#pragma once

#include <Wt/Chart/WCartesianChart.h>
#include <Wt/WComboBox.h>

#include "BaseDeviceWidget.hpp"
#include "ExtendedThermometerCurrentValue.hpp"
#include "PeriodSettings.hpp"
#include "TemperatureChartModel.hpp"
#include "ThermometerLedBrightness.hpp"

class ThermometerWidget final : public BaseDeviceWidget {
public:
    ThermometerWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~ThermometerWidget() = default;

protected:
    virtual void OnBack() override;

    virtual void Initialize() override;

    virtual void ClearData() override;

    ThermometerLedBrightness GetBrightness();

    virtual void OnSettingsButton() override;

private:
    Wt::WText* _temperatureText;
    Wt::Chart::WCartesianChart* _chart;
    std::shared_ptr<TemperatureChartModel> _model;
    Wt::WComboBox* _seconds;
    std::vector<ExtendedThermometerCurrentValue> _cachedValues;
};
