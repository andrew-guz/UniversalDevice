#ifndef _THERMOMETER_WIDGET_H_
#define _THERMOMETER_WIDGET_H_

#include <Wt/Chart/WCartesianChart.h>

#include "BaseDeviceWidget.h"
#include "TemperatureChartModel.h"
#include "ThermometerSettings.h"
#include "ExtendedThermometerCurrentValue.h"
#include "ThermometerLedBrightness.h"

class ThermometerWidget final : public BaseDeviceWidget
{
public:
    ThermometerWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~ThermometerWidget() = default;

protected:
    virtual void Initialize() override;

    virtual void ClearData() override;
    
    ThermometerSettings GetSettings();

    std::vector<ExtendedThermometerCurrentValue> GetValues();

    ThermometerLedBrightness GetBrightness();

    virtual void OnSettingsButton() override;

private:
    Wt::WText*                              _temperatureText;
    Wt::Chart::WCartesianChart*             _chart;
    std::shared_ptr<TemperatureChartModel>  _model;
};

#endif //_THERMOMETER_WIDGET_H_
