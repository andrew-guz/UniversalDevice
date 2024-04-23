#include "DevicesWidget.hpp"

#include <Wt/Http/Cookie.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPopupMenu.h>

#include "Constants.hpp"
#include "Defines.hpp"
#include "DeviceButton.hpp"
#include "JsonExtension.hpp"
#include "Logger.hpp"
#include "MessageHelper.hpp"
#include "RequestHelper.hpp"
#include "UrlHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

DevicesWidget::DevicesWidget(IStackHolder* stackHolder, const Settings& settings) : BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, 0, 1, 5);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WGridLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto exitButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Выход"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(exitButton);
    exitButton->clicked().connect([&]() {
        WApplication::instance()->removeCookie(Http::Cookie{"authorization"});
        _stackHolder->SetWidget(StackWidgetType::Login, {});
    });

    auto eventsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("События"), 0, 1, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(eventsButton);
    eventsButton->clicked().connect([&]() { _stackHolder->SetWidget(StackWidgetType::Events, {}); });

    auto logsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Логи"), 0, 2, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(logsButton);
    logsButton->clicked().connect([&]() { _stackHolder->SetWidget(StackWidgetType::Logs, {}); });

    auto refreshButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 3, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([&]() { Refresh(); });

    Refresh();
}

void DevicesWidget::Initialize(const std::string& data) { Refresh(); }

void DevicesWidget::Clear() {
    for (auto iter = _deviceWidgets.begin(); iter != _deviceWidgets.end(); ++iter) {
        iter->second->removeWidget(iter->first);
    }
    _deviceWidgets.clear();
}

void DevicesWidget::Refresh() {
    Clear();
    auto replyJson = RequestHelper::DoGetRequest({BACKEND_IP, _settings._servicePort, API_CLIENT_DEVICES}, Constants::LoginService);
    auto allDescriptions = JsonExtension::CreateVectorFromJson<ExtendedComponentDescription>(replyJson);
    if (allDescriptions.empty())
        return;
    LOG_DEBUG << allDescriptions.size() << " allDescriptions found." << std::endl;
    std::set<std::string> groups;
    std::for_each(allDescriptions.begin(), allDescriptions.end(), [&groups](const auto& d) {
        if (d._group.size())
            groups.insert(d._group);
    });
    auto groupRow = 2;
    auto buttonRow = 0;
    auto buttonColumn = 0;
    std::vector<ExtendedComponentDescription> currentDescriptions;
    for (auto& group : groups) {
        auto groupGroupBox = _mainLayout->addWidget(std::make_unique<WGroupBox>(group), groupRow++, 0, 1, 5);
        auto groupLayout = groupGroupBox->setLayout(std::make_unique<WGridLayout>());
        groupLayout->setVerticalSpacing(30);
        buttonRow = 0;
        buttonColumn = 0;
        currentDescriptions.clear();
        std::copy_if(allDescriptions.begin(), allDescriptions.end(), std::back_inserter(currentDescriptions), [](const auto& d) { return d._group.size(); });
        std::sort(currentDescriptions.begin(), currentDescriptions.end(), [](const auto& a, const auto& b) { return a._name.compare(b._name) < 0; });
        for (auto& description : currentDescriptions) {
            if (description._group != group)
                continue;
            AddButtonToLayout(groupLayout, description, buttonRow, buttonColumn);
        }
        _deviceWidgets.insert(std::make_pair(groupGroupBox, _mainLayout));
    }
    buttonRow = groupRow;
    buttonColumn = 0;
    currentDescriptions.clear();
    std::copy_if(allDescriptions.begin(), allDescriptions.end(), std::back_inserter(currentDescriptions), [](const auto& d) { return d._group.size() == 0; });
    std::sort(currentDescriptions.begin(), currentDescriptions.end(), [](const auto& a, const auto& b) { return a._name.compare(b._name) < 0; });
    for (auto& description : allDescriptions) {
        if (description._group.size())
            continue;
        auto button = AddButtonToLayout(_mainLayout, description, buttonRow, buttonColumn);
        _deviceWidgets.insert(std::make_pair(button, _mainLayout));
    }
}

DeviceButton* DevicesWidget::AddButtonToLayout(WGridLayout* layout, const ExtendedComponentDescription& description, int& row, int& column) {
    auto button = layout->addWidget(std::make_unique<DeviceButton>(_settings._servicePort, description), row, column, AlignmentFlag::Top | AlignmentFlag::Center);
    button->clicked().connect([this, description]() {
        if (description._type == Constants::DeviceTypeThermometer)
            _stackHolder->SetWidget(StackWidgetType::Thermometer, description._id.data());
        if (description._type == Constants::DeviceTypeRelay)
            _stackHolder->SetWidget(StackWidgetType::Relay, description._id.data());
        if (description._type == Constants::DeviceTypeMotionRelay)
            _stackHolder->SetWidget(StackWidgetType::MotionRelay, description._id.data());
    });
    button->mouseWentUp().connect([this, description](const WMouseEvent& event) {
        if (event.button() == MouseButton::Right) {
            auto popup = std::make_unique<Wt::WPopupMenu>();
            auto deleteItem = popup->addItem("Удалить...");
            deleteItem->triggered().connect([this, description]() {
                auto result = RequestHelper::DoDeleteRequest({BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE, "<string>", description._id.data())}, Constants::LoginService, {});
                if (result == 200)
                    Refresh();
                else
                    LOG_ERROR << "Failed to delete device " << description._id.data() << "." << std::endl;
            });
            popup->exec(event);
        }
    });
    ++column;
    if (column == 5) {
        ++row;
        column = 0;
    }
    return button;
}
