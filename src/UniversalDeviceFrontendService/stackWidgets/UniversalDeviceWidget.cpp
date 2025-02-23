#include "UniversalDeviceWidget.hpp"

#include <Wt/WDialog.h>

#include "Logger.hpp"
#include "PeriodSettings.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

UniversalDeviceWidget::UniversalDeviceWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings) {}

void UniversalDeviceWidget::OnBack() { BaseDeviceWidget::OnBack(); }

void UniversalDeviceWidget::Initialize() {}

void UniversalDeviceWidget::ClearData() {}

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
