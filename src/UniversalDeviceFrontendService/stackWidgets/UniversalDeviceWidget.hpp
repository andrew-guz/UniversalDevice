#pragma once

#include <Wt/Chart/WCartesianChart.h>

#include "BaseDeviceWidget.hpp"

class UniversalDeviceWidget final : public BaseDeviceWidget {
public:
    UniversalDeviceWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~UniversalDeviceWidget() = default;

protected:
    using BaseStackWidget::Initialize;

    virtual void OnBack() override;

    virtual void Initialize() override;

    virtual void ClearData() override;

    virtual void OnSettingsButton() override;

private:
};
