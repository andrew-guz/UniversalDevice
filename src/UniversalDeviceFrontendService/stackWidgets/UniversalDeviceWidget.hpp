#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <Wt/WTabWidget.h>

#include "ApplicationSettings.hpp"
#include "BaseDeviceWidget.hpp"
#include "SecondsComboBox.hpp"
#include "UniversalDeviceChartModel.hpp"

class UniversalDeviceWidget final : public BaseDeviceWidget {
public:
    UniversalDeviceWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

    virtual ~UniversalDeviceWidget() = default;

protected:
    using BaseStackWidget::Initialize;

    virtual void OnBack() override;

    virtual void Initialize() override;

    virtual void ClearData() override;

    virtual void OnSettingsButton() override;

private:
    Wt::WTabWidget* _tabWidget;
    std::set<std::string> _knownParameters;
    std::vector<std::shared_ptr<UniversalDeviceChartModel>> _models;
    std::unordered_map<std::string, Wt::WText*> _texts;
    SecondsComboBox* _seconds;
    std::vector<ExtendedUniversalDeviceCurrentValues> _cachedValues;
};
