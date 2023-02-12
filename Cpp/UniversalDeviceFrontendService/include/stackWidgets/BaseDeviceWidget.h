#ifndef _BASE_DEVICE_WIDGET_H_
#define _BASE_DEVICE_WIDGET_H_

#include <mutex>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WText.h>
#include <Wt/WTimer.h>

#include "Uuid.h"
#include "BaseStackWidget.h"

class BaseDeviceWidget : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    BaseDeviceWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~BaseDeviceWidget() = default;

    virtual void Initialize(const std::string& data) override final;

protected:
    virtual void Initialize() = 0;

    enum ClearType
    {
        Name = 0x1,
        Data = 0x2,
        All = Name | Data
    };

    void Clear(ClearType type);

    virtual void ClearData() = 0;

    void UpdateName();

    virtual void OnSettingsButton() = 0;

protected:
    std::mutex          _requestMutex;
    Wt::WGridLayout*    _mainLayout;
    Wt::WText*          _nameText;
    Wt::WText*          _timeText;
    Uuid                _deviceId;
    std::string         _deviceName;
    Wt::WTimer*         _refreshTimer;
};

#endif //_BASE_DEVICE_WIDGET_H_
