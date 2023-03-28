#include "DevicesWidget.h"

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "ExtendedComponentDescription.h"
#include "WidgetHelper.h"

using namespace Wt;

DevicesWidget::DevicesWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto exitButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Выход"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(exitButton);
    exitButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Login, {});
    });

    auto eventsButton = _mainLayout->addWidget(std::make_unique<WPushButton>("События"), 0, 1, 1, 3, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(eventsButton);
    eventsButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Events, {});
    });
    
    auto refreshButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 4, AlignmentFlag::Right);
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
    auto replyJson = RequestHelper::DoGetRequest({"127.0.0.1", _settings._servicePort, API_CLIENT_DEVICES}, Constants::LoginService);
    auto descriptions = JsonExtension::CreateVectorFromJson<ExtendedComponentDescription>(replyJson);
    if (descriptions.empty())
        return;
    LOG_DEBUG << descriptions.size() << " descriptions found." << std::endl;
    std::sort(descriptions.begin(), descriptions.end(), [](const auto& a, const auto& b){ return a._name.compare(b._name) < 0; });
    int row = 2;
    int column = 0;
    for (auto& description : descriptions)
    {
        auto button = _mainLayout->addWidget(std::make_unique<WPushButton>(), row, column, AlignmentFlag::Top | AlignmentFlag::Center);
        button->setText(description._name.size() ? description._name : description._id.data());
        button->setMinimumSize(200, 150);
        button->setMaximumSize(200, 150);
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
