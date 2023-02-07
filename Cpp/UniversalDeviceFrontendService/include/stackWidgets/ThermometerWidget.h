#ifndef _THERMOMETER_WIDGET_H_
#define _THERMOMETER_WIDGET_H_

#include <mutex>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WText.h>
#include <Wt/Chart/WCartesianChart.h>

#include "Uuid.h"
#include "BaseStackWidget.h"
#include "TemperatureChartModel.h"
#include "ThermometerSettings.h"
#include "ExtendedThermometerCurrentValue.h"

class ThermometerWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    ThermometerWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~ThermometerWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    enum ClearType
    {
        Name = 0x1,
        Temperature = 0x2,
        All = Name | Temperature 
    };

    void Clear(ClearType type);

    void UpdateName();

    ThermometerSettings GetSettings();

    std::vector<ExtendedThermometerCurrentValue> GetValues();

    void OnSettings();

private:
    std::mutex                              _requestMutex;
    Wt::WGridLayout*                        _mainLayout;
    Wt::WText*                              _nameText;
    Wt::WText*                              _temperatureText;
    Wt::WText*                              _temperatureTimeText;
    Wt::Chart::WCartesianChart*             _chart;
    std::shared_ptr<TemperatureChartModel>  _model;
    Uuid                                    _deviceId;
    std::string                             _thermometerName;
};

#endif //_THERMOMETER_WIDGET_H_
