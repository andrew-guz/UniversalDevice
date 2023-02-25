#ifndef _MOTION_RELAY_WIDGET_H_
#define _MOTION_RELAY_WIDGET_H_

#include "BaseDeviceWidget.h"
#include "MotionRelaySettings.h"
#include "ExtendedMotionRelayCurrentState.h"

class MotionRelayWidget final : public BaseDeviceWidget
{
public:
    MotionRelayWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~MotionRelayWidget() = default;

protected:
    using BaseStackWidget::Initialize;

    virtual void Initialize() override;

    virtual void ClearData() override;

    virtual void OnSettingsButton() override;

    void OnStateButton();

private:
    Wt::WText*          _motionText;
    Wt::WText*          _stateText;
    Wt::WPushButton*    _stateButton;
    bool                _motionDetected = false;
    int                 _deviceState = 0;
};

#endif //_MOTION_RELAY_WIDGET_H_
