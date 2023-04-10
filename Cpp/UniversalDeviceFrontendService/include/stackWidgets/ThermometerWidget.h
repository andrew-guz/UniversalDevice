#ifndef _THERMOMETER_WIDGET_H_
#define _THERMOMETER_WIDGET_H_

#include <Wt/Chart/WCartesianChart.h>
#include <Wt/WComboBox.h>

#include "BaseDeviceWidget.h"
#include "TemperatureChartModel.h"
#include "PeriodSettings.h"
#include "ExtendedThermometerCurrentValue.h"
#include "ThermometerLedBrightness.h"

class ThermometerWidget final : public BaseDeviceWidget
{
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
    Wt::WText*                                      _temperatureText;
    Wt::Chart::WCartesianChart*                     _chart;
    std::shared_ptr<TemperatureChartModel>          _model;
    Wt::WComboBox*                                  _seconds;
    std::vector<ExtendedThermometerCurrentValue>    _cachedValues;
};

#endif //_THERMOMETER_WIDGET_H_
