#pragma once

#include "BaseDeviceWidget.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "RelayChartModel.hpp"
#include "SecondsComboBox.hpp"

class RelayWidget final : public BaseDeviceWidget {
public:
    RelayWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~RelayWidget() = default;

protected:
    using BaseStackWidget::Initialize;

    virtual void OnBack() override;

    virtual void Initialize() override;

    virtual void ClearData() override;

    virtual void OnSettingsButton() override;

    void OnStateButton();

private:
    Wt::WText* _stateText;
    Wt::WPushButton* _stateButton;
    int _deviceState = 0;
    std::shared_ptr<RelayChartModel> _model;
    SecondsComboBox* _seconds;
    std::vector<ExtendedRelayCurrentState> _cachedValues;
};
