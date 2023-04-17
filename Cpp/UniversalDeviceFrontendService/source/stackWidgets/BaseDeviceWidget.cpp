#include "BaseDeviceWidget.h"

#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSpinBox.h>
#include <Wt/WRegExpValidator.h>

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "ComponentDescription.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "UrlHelper.h"
#include "DeviceProperty.h"
#include "WidgetHelper.h"

using namespace Wt;

BaseDeviceWidget::BaseDeviceWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings),
    _deviceId(Uuid::Empty())
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([&](){
        OnBack();
        _stackHolder->SetWidget(StackWidgetType::Devices, {});
        _deviceId = Uuid::Empty();
    });

    auto settingsButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Настройки"), 0, 1, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(settingsButton);
    settingsButton->clicked().connect([&](){
        OnSettingsButton();
    });

    auto refresh = _mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 2, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refresh);
    refresh->clicked().connect([&](){
        Initialize(_deviceId.data());
    });

    _nameText = _mainLayout->addWidget(std::make_unique<WText>(), 1, 0, 1, 3, AlignmentFlag::Center);
    _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));

    _timeText = _mainLayout->addWidget(std::make_unique<WText>(), 2, 0, 1, 3, AlignmentFlag::Center);
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    
    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([&](){
        Initialize(_deviceId.data());
    });
    _refreshTimer->start();

    _mainLayout->setVerticalSpacing(20);
    _mainLayout->setRowStretch(0, 0);
    _mainLayout->setRowStretch(1, 0);
    _mainLayout->setRowStretch(2, 0);
}

void BaseDeviceWidget::Initialize(const std::string& data)
{
    std::lock_guard<std::mutex> lockGuard(_requestMutex);

    _deviceId =  Uuid(data);
    if (_deviceId.isEmpty())
    {
        Clear(BaseDeviceWidget::ClearType::All);
        return;
    }

    GetDeviceProperty(API_CLIENT_DEVICE_NAME, _deviceName);
    _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
    GetDeviceProperty(API_CLIENT_DEVICE_GROUP, _deviceGroup);

    Initialize();
}

void BaseDeviceWidget::Clear(ClearType type)
{
    if (type & BaseDeviceWidget::ClearType::Name)
    {
        _deviceName = {};
        _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
        _deviceGroup = {};
    }
    if (type & BaseDeviceWidget::ClearType::Data)
    {
        ClearData();
    }
}
    
void BaseDeviceWidget::GetDeviceProperty(const std::string& path, std::string& value)
{
    auto replyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(path, "<string>", _deviceId.data()) }, Constants::LoginService);
    value = JsonExtension::CreateFromJson<DeviceProperty>(replyJson)._value;
}

bool BaseDeviceWidget::SetDeviceProperty(const std::string& path, const std::string& newValue, std::string& value)
{
    DeviceProperty deviceProperty;
    deviceProperty._value = newValue;
    auto result = RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(path, "<string>", _deviceId.data()) }, Constants::LoginService, deviceProperty.ToJson());
    if (result == 200)
    {
        value = newValue;
        return true;
    }
    LOG_ERROR << "Failed to update property " << path << " to " << newValue << "." << std::endl;
    return false;
}

void BaseDeviceWidget::SetNewName(const std::string& newName)
{
    if (SetDeviceProperty(API_CLIENT_DEVICE_NAME, newName, _deviceName))
        _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
}

void BaseDeviceWidget::SetNewGroup(const std::string& newGroup)
{
    SetDeviceProperty(API_CLIENT_DEVICE_GROUP, newGroup, _deviceGroup);
}