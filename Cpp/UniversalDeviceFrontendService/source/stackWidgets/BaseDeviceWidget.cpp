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
#include "DeviceName.h"
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
        _stackHolder->SetWidget(StackWidgetType::Devices, "");
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

    _nameText = _mainLayout->addWidget(std::make_unique<WText>(), 1, 1, AlignmentFlag::Center);
    _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));

    _timeText = _mainLayout->addWidget(std::make_unique<WText>(), 2, 1, AlignmentFlag::Center);
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    
    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([&]()
    {
        Initialize(_deviceId.data());
    });
    _refreshTimer->start();
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

    UpdateName();
    Initialize();
}

void BaseDeviceWidget::Clear(ClearType type)
{
    if (type & BaseDeviceWidget::ClearType::Name)
    {
        _deviceName = {};
        _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
    }
    if (type & BaseDeviceWidget::ClearType::Data)
    {
        ClearData();
    }
}
    
void BaseDeviceWidget::UpdateName()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_CLIENT_DEVICE_NAME, "<string>", _deviceId.data()) }, Constants::LoginService);
    _deviceName = JsonExtension::CreateFromJson<DeviceName>(replyJson)._name;
    _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
}
