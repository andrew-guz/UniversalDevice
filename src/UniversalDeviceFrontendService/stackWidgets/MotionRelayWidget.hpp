#pragma once

#include <Wt/WGlobal.h>
#include <Wt/WText.h>

#include "ApplicationSettings.hpp"
#include "BaseDeviceWidget.hpp"
#include "BaseStackWidget.hpp"
#include "IStackHolder.hpp"

class MotionRelayWidget final : public BaseDeviceWidget {
public:
    MotionRelayWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

    virtual ~MotionRelayWidget() = default;

protected:
    using BaseStackWidget::Initialize;

    virtual void Initialize() override;

    virtual void ClearData() override;

    virtual void OnSettingsButton() override;

    void OnStateButton();

private:
    Wt::WText* _motionText;
    Wt::WText* _stateText;
    Wt::WPushButton* _stateButton;
    bool _motionDetected = false;
    int _deviceState = 0;
};
