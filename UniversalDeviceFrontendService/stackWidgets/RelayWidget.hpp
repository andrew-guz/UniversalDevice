#pragma once

#include "BaseDeviceWidget.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "PeriodSettings.hpp"

class RelayWidget final : public BaseDeviceWidget {
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
    Wt::WText* _stateText;
    Wt::WPushButton* _stateButton;
    int _deviceState = 0;
};
