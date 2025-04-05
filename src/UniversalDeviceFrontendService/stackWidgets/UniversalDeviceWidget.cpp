#include "UniversalDeviceWidget.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Wt/Chart/WCartesianChart.h>
#include <Wt/WAbstractItemModel.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WDialog.h>
#include <Wt/WGlobal.h>
#include <Wt/WTabWidget.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WWidget.h>
#include <fmt/format.h>

#include "ChartFactory.hpp"
#include "Enums.hpp"
#include "ExtendedUniversalDeviceCurrentValues.hpp"
#include "Logger.hpp"
#include "PeriodSettings.hpp"
#include "TimeHelper.hpp"
#include "UniversalDeviceChartModel.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

namespace {
    std::set<std::string> findParameters(const std::vector<ExtendedUniversalDeviceCurrentValues>& values) {
        std::set<std::string> result;
        for (const ExtendedUniversalDeviceCurrentValues& value : values) {
            std::set<std::string> current;
            for (const auto& [key, _] : value._values)
                current.emplace(key);

            if (result.empty())
                result = std::move(current);
            else
                current.merge(result);
        }

        return result;
    }

    std::shared_ptr<UniversalDeviceChartModel> createModel(const std::vector<ExtendedUniversalDeviceCurrentValues>& values,
                                                           const std::string& parameter) {
        switch (values.front()._values.at(parameter).GetType()) {
            case UniversalDataType::Bool:
            case UniversalDataType::Int:
            case UniversalDataType::Double:
                return std::make_shared<UniversalDeviceChartModel>(values, parameter);
            case UniversalDataType::String:
                return nullptr;
        }
        return nullptr;
    }

    std::unique_ptr<WContainerWidget> createChart(const std::shared_ptr<UniversalDeviceChartModel>& model, const UniversalDataType type) {
        auto canvas = std::make_unique<WContainerWidget>();
        auto canvasLayout = canvas->setLayout(std::make_unique<WVBoxLayout>());
        bool drawGridLines = false;
        Chart::SeriesType seriesType = Chart::SeriesType::Line;
        switch (type) {
            case UniversalDataType::Bool:
                break;
            case UniversalDataType::Int:
                drawGridLines = true;
                seriesType = Chart::SeriesType::Curve;
                break;
            case UniversalDataType::Double:
                drawGridLines = true;
                seriesType = Chart::SeriesType::Curve;
                break;
            case UniversalDataType::String:
                assert(false);
                break;
        }
        canvasLayout->addWidget(
            std::unique_ptr<Chart::WCartesianChart>(ChartFactory::CreateChart(model, drawGridLines, seriesType, WColor(255, 165, 0, 255))), 1);
        return canvas;
    }

    std::pair<WContainerWidget*, WText*> createText() {
        auto canvas = new WContainerWidget();
        auto canvasLayout = canvas->setLayout(std::make_unique<WVBoxLayout>());
        auto text = canvasLayout->addWidget(std::make_unique<WText>(), 1);
        return std::make_pair(canvas, text);
    }
} // namespace

UniversalDeviceWidget::UniversalDeviceWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings) {
    _tabWidget = _mainLayout->addWidget(std::make_unique<WTabWidget>(), 1);
    _mainLayout->addWidget(std::make_unique<WText>("За последние:"), 0, AlignmentFlag::Center | AlignmentFlag::Bottom);
    _seconds = _mainLayout->addWidget(std::make_unique<SecondsComboBox>(), 0, AlignmentFlag::Center | AlignmentFlag::Bottom);
    _seconds->changed().connect([this]() {
        _cachedValues.clear();
        BaseDeviceWidget::Initialize(_deviceId.data());
    });
}

void UniversalDeviceWidget::OnBack() {
    _seconds->setCurrentIndex(1);
    _cachedValues.clear();
    ClearData();
    BaseDeviceWidget::OnBack();
}

void UniversalDeviceWidget::Initialize() {
    const uint64_t seconds = _seconds->GetSeconds();
    if (_cachedValues.empty())
        _cachedValues = GetValues<ExtendedUniversalDeviceCurrentValues>(DeviceType::UniversalDevice, seconds);
    else {
        auto lastValues = GetValues<ExtendedUniversalDeviceCurrentValues>(DeviceType::UniversalDevice, 0);
        if (lastValues.empty() ||
            lastValues[0]._timestamp != std::max_element(_cachedValues.begin(), _cachedValues.end(), [](const auto& a, const auto& b) {
                                            return a._timestamp < b._timestamp;
                                        })->_timestamp)
            _cachedValues = GetValues<ExtendedUniversalDeviceCurrentValues>(DeviceType::UniversalDevice, seconds);
    }
    if (_cachedValues.size()) {
        const std::set<std::string> parameters = findParameters(_cachedValues);
        if (parameters != _knownParameters) {
            Clear(BaseDeviceWidget::ClearType::Data);
            _knownParameters = parameters;
            for (const std::string& parameter : _knownParameters) {
                const UniversalDataType type = _cachedValues.front()._values.at(parameter).GetType();
                switch (type) {
                    case UniversalDataType::Bool:
                    case UniversalDataType::Int:
                    case UniversalDataType::Double: {
                        const auto model = createModel(_cachedValues, parameter);
                        if (!model)
                            continue;
                        _models.push_back(model);
                        _tabWidget->addTab(createChart(model, type), parameter);
                    } break;
                    case UniversalDataType::String: {
                        auto [canvas, text] = createText();
                        _texts.try_emplace(parameter, text);
                        _tabWidget->addTab(std::unique_ptr<WContainerWidget>(canvas), parameter);
                    } break;
                }
            }
        }

        auto timestamp = _cachedValues.begin()->_timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));

        for (const auto& model : _models)
            model->SetData(_cachedValues);

        std::unordered_map<std::string, std::string> texts;
        for (const auto& extendedValue : _cachedValues) {
            for (const auto& [parameter, value] : extendedValue._values) {
                if (value.GetType() != UniversalDataType::String)
                    continue;
                texts[parameter] += fmt::format("[<i>{}</i>]\t{}<br/>", TimeHelper::TimeToString(extendedValue._timestamp), value.Get<std::string>());
            }
        }

        for (const auto& [parameter, text] : _texts) {
            const auto iter = texts.find(parameter);
            if (iter != texts.end())
                text->setText(iter->second);
            else
                text->setText({});
        }
    } else
        Clear(BaseDeviceWidget::ClearType::Data);
}

void UniversalDeviceWidget::ClearData() {
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    while (_tabWidget->count())
        _tabWidget->removeTab(_tabWidget->widget(0));
    _knownParameters.clear();
    _models.clear();
    _texts.clear();
}

void UniversalDeviceWidget::OnSettingsButton() {
    if (_deviceId.isEmpty())
        return;
    auto settings = GetSettings<PeriodSettings>();
    auto [dialog, layout, nameEdit, groupEdit, periodEdit, ok] = WidgetHelper::CreateBaseSettingsDialog(
        this, 180, _deviceName, _deviceGroup, settings._period, true, std::bind(&UniversalDeviceWidget::onRestart, this));
    // execute
    if (dialog->exec() != DialogCode::Accepted)
        return;
    // update name
    auto newName = nameEdit->text().toUTF8();
    if (newName.size())
        SetNewName(newName);
    // update group
    SetNewGroup(groupEdit->text().toUTF8());
    // update settings
    PeriodSettings newSettings;
    newSettings._period = periodEdit->value() * 1000;
    auto result =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) },
                                     Constants::LoginService,
                                     newSettings);
    if (result != 200)
        LOG_ERROR_MSG(fmt::format("Failed to update settings to {}", nlohmann::json(newSettings).dump()));
}
