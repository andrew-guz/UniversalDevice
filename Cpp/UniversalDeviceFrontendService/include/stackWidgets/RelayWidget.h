#ifndef _RELAY_WIDGET_H_
#define _RELAY_WIDGET_H_

#include "BaseDeviceWidget.h"
#include "PeriodSettings.h"
#include "ExtendedRelayCurrentState.h"

class RelayWidget final : public BaseDeviceWidget
{
public:
    RelayWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~RelayWidget() = default;

protected:
    using BaseStackWidget::Initialize;

    virtual void Initialize() override;

    virtual void ClearData() override;

    virtual void OnSettingsButton() override;

    void OnStateButton();

private:
    Wt::WText*          _stateText;
    Wt::WPushButton*    _stateButton;
    int                 _deviceState = 0;
};

#endif //_RELAY_WIDGET_H_
