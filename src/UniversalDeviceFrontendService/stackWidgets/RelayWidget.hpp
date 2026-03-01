#pragma once

#include <memory>
#include <vector>

#include <Wt/WGlobal.h>
#include <Wt/WText.h>

#include "ApplicationSettings.hpp"
#include "BaseDeviceWidget.hpp"
#include "BaseStackWidget.hpp"
#include "IStackHolder.hpp"
#include "RelayChartModel.hpp"
#include "RelayValue.hpp"
#include "SecondsComboBox.hpp"

class RelayWidget final : public BaseDeviceWidget {
public:
    RelayWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

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
    std::vector<RelayValue> _cachedValues;
};
