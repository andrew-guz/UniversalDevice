#include "BaseDeviceWidget.hpp"

#include <Wt/WDialog.h>
#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WRegExpValidator.h>
#include <Wt/WSpinBox.h>
#include <Wt/WVBoxLayout.h>
#include <fmt/format.h>

#include "ApplicationSettings.hpp"
#include "Constants.hpp"
#include "Defines.hpp"
#include "DeviceProperty.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "UrlHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

namespace {

    std::string CombineNameAndGroup(const std::string name, const std::string& group) {
        return group.empty() ? name : name + std::string{ " (" } + group + std::string{ ")" };
    }

} // namespace

BaseDeviceWidget::BaseDeviceWidget(IStackHolder* stackHolder, const ApplicationSettings& settings) :
    BaseStackWidget(stackHolder, settings),
    _deviceId(Uuid::Empty()) {
    _mainLayout = setLayout(std::make_unique<WVBoxLayout>());
    _mainLayout->setSpacing(20);

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WHBoxLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto backButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() {
        OnBack();
        _stackHolder->SetWidget(StackWidgetType::Devices, {});
        _deviceId = Uuid::Empty();
    });

    auto settingsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Настройки"), 0, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(settingsButton);
    settingsButton->clicked().connect([this]() { OnSettingsButton(); });

    auto refresh = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refresh);
    refresh->clicked().connect([this]() { Initialize(_deviceId.data()); });

    _nameText = _mainLayout->addWidget(std::make_unique<WText>(), 0, AlignmentFlag::Center | AlignmentFlag::Top);
    _nameText->setText(WidgetHelper::TextWithFontSize(CombineNameAndGroup(_deviceName, _deviceGroup), 20));

    _timeText = _mainLayout->addWidget(std::make_unique<WText>(), 0, AlignmentFlag::Center | AlignmentFlag::Top);
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));

    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([this]() { Initialize(_deviceId.data()); });
    _refreshTimer->start();
}

void BaseDeviceWidget::Initialize(const std::string& data) {
    std::lock_guard<std::mutex> lockGuard(_requestMutex);

    _deviceId = Uuid(data);
    if (_deviceId.isEmpty()) {
        Clear(BaseDeviceWidget::ClearType::All);
        return;
    }

    GetDeviceProperty(API_CLIENT_DEVICE_NAME, _deviceName);
    GetDeviceProperty(API_CLIENT_DEVICE_GROUP, _deviceGroup);
    _nameText->setText(WidgetHelper::TextWithFontSize(CombineNameAndGroup(_deviceName, _deviceGroup), 20));

    Initialize();
}

void BaseDeviceWidget::Clear(ClearType type) {
    if (type & BaseDeviceWidget::ClearType::Name) {
        _deviceName = {};
        _deviceGroup = {};
        _nameText->setText(WidgetHelper::TextWithFontSize(CombineNameAndGroup(_deviceName, _deviceGroup), 20));
    }
    if (type & BaseDeviceWidget::ClearType::Data) {
        ClearData();
    }
}

void BaseDeviceWidget::GetDeviceProperty(const std::string& path, std::string& value) {
    auto replyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(path, "<string>", _deviceId.data()) },
                                                 Constants::LoginService);
    value = !replyJson.is_null() ? replyJson.get<DeviceProperty>()._value : std::string{};
}

bool BaseDeviceWidget::SetDeviceProperty(const std::string& path, const std::string& newValue, std::string& value) {
    DeviceProperty deviceProperty;
    deviceProperty._value = newValue;
    auto result = RequestHelper::DoPostRequest(
        { BACKEND_IP, _settings._servicePort, UrlHelper::Url(path, "<string>", _deviceId.data()) }, Constants::LoginService, deviceProperty);
    if (result == 200) {
        value = newValue;
        return true;
    }
    LOG_ERROR_MSG(fmt::format("Failed to update property {} to {}", path, newValue));
    return false;
}

void BaseDeviceWidget::SetNewName(const std::string& newName) {
    if (SetDeviceProperty(API_CLIENT_DEVICE_NAME, newName, _deviceName))
        _nameText->setText(WidgetHelper::TextWithFontSize(CombineNameAndGroup(_deviceName, _deviceGroup), 20));
}

void BaseDeviceWidget::SetNewGroup(const std::string& newGroup) {
    if (SetDeviceProperty(API_CLIENT_DEVICE_GROUP, newGroup, _deviceGroup))
        _nameText->setText(WidgetHelper::TextWithFontSize(CombineNameAndGroup(_deviceName, _deviceGroup), 20));
}

void BaseDeviceWidget::onRestart() {
    const auto result = RequestHelper::DoPostRequest(
        { BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_CLIENT_RESTART_DEVICE, "<string>", _deviceId.data()) }, Constants::LoginService, {});
    if (result == 200)
        WidgetHelper::ShowSimpleMessage(this, "Информация", "Устройство перезагружено.", 5000);
    else
        WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Ошибка перезагрузки устройства.", 5000);
}
