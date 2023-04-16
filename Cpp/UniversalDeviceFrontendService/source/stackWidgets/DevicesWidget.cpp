#include "DevicesWidget.h"

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "JsonExtension.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "ExtendedComponentDescription.h"
#include "WidgetHelper.h"

using namespace Wt;

DevicesWidget::DevicesWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    _mainLayout->setVerticalSpacing(50);

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, 0, 1, 5);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WGridLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto exitButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Выход"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(exitButton);
    exitButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Login, {});
    });

    auto eventsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("События"), 0, 1, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(eventsButton);
    eventsButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Events, {});
    });

    auto logsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Логи"), 0, 2, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(logsButton);
    logsButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Logs, {});
    });
    
    auto refreshButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 3, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([&](){
        Refresh();
    });

    Refresh();
}

void DevicesWidget::Initialize(const std::string& data)
{
    Refresh();
}

void DevicesWidget::Clear()
{
    for(auto& button : _deviceButtons)
        _mainLayout->removeWidget(button);
    _deviceButtons.clear();
}

void DevicesWidget::Refresh()
{
    Clear();
    auto replyJson = RequestHelper::DoGetRequest({BACKEND_IP, _settings._servicePort, API_CLIENT_DEVICES}, Constants::LoginService);
    auto descriptions = JsonExtension::CreateVectorFromJson<ExtendedComponentDescription>(replyJson);
    if (descriptions.empty())
        return;
    LOG_DEBUG << descriptions.size() << " descriptions found." << std::endl;
    std::sort(descriptions.begin(), descriptions.end(), [](const auto& a, const auto& b){ return a._name.compare(b._name) < 0; });
    int row = 2;
    int column = 0;
    for (auto& description : descriptions)
    {
        auto button = _mainLayout->addWidget(std::make_unique<DeviceButton>(_settings._servicePort, description), row, column, AlignmentFlag::Top | AlignmentFlag::Center);
        button->clicked().connect([description, this](){
            if (description._type == Constants::DeviceTypeThermometer)
                _stackHolder->SetWidget(StackWidgetType::Thermometer, description._id.data());
            if (description._type == Constants::DeviceTypeRelay)
                _stackHolder->SetWidget(StackWidgetType::Relay, description._id.data());
            if (description._type == Constants::DeviceTypeMotionRelay)
                _stackHolder->SetWidget(StackWidgetType::MotionRelay, description._id.data());
        });
        _deviceButtons.push_back(button);
        ++column;
        if (column == 5)
        {
            ++row;
            column = 0;
        }
    }
}
