#include "DevicesWidget.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include <Wt/Http/Cookie.h>
#include <Wt/WGlobal.h>
#include <Wt/WGridLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <fmt/format.h>

#include "Constants.hpp"
#include "Defines.hpp"
#include "DeviceButton.hpp"
#include "Enums.hpp"
#include "ExtendedComponentDescription.hpp"
#include "IStackHolder.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "Scenario.hpp"
#include "String.hpp"
#include "UrlHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

namespace {
    struct Group {
        Group* _parent;
        std::vector<std::shared_ptr<Group>> _children;
        std::string _name;
        std::vector<ExtendedComponentDescription> _descriptions;
    };
} // namespace

DevicesWidget::DevicesWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, 0, 1, 5);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WGridLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto exitButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Выход"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(exitButton);
    exitButton->clicked().connect([this]() {
        WApplication::instance()->removeCookie(Http::Cookie{ "authorization" });
        _stackHolder->SetWidget(StackWidgetType::Login, {});
    });

    auto eventsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("События"), 0, 1, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(eventsButton);
    eventsButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Events, {}); });

    auto scenariosButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Сценарии"), 0, 2, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(scenariosButton);
    scenariosButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Scenarios, {}); });

    auto logsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Логи"), 0, 3, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(logsButton);
    logsButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Logs, {}); });

    auto refreshButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 4, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

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
    const auto scenariosReplyJson =
        RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_SCENARIOS }, Constants::LoginService);
    const auto scenarios = !scenariosReplyJson.is_null() ? scenariosReplyJson.get<std::vector<Scenario>>() : std::vector<Scenario>{};

    const auto devicesReplyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_DEVICES }, Constants::LoginService);
    const auto allDescriptions =
        !devicesReplyJson.is_null() ? devicesReplyJson.get<std::vector<ExtendedComponentDescription>>() : std::vector<ExtendedComponentDescription>{};
    if (allDescriptions.empty())
        return;
    LOG_DEBUG_MSG(fmt::format("{} descriptions found", allDescriptions.size()));

    if (scenarios.size()) {
        auto scenariosWidget = _mainLayout->addWidget(std::make_unique<WGroupBox>("Сценарии"), 2, 0, 1, 5, AlignmentFlag::Top);
        auto scenariosLayout = scenariosWidget->setLayout(std::make_unique<WGridLayout>());
        int scenarioRow = 0;
        int scenarioColumn = 0;
        for (const auto& scenario : scenarios) {
            AddScenarioButton(scenariosLayout, scenario, scenarioRow, scenarioColumn);
        }
    }

    std::set<std::string> groups;
    std::vector<ExtendedComponentDescription> descriptionsWithoutGroup;
    std::vector<ExtendedComponentDescription> descriptionsWithGroup;
    std::for_each(allDescriptions.begin(), allDescriptions.end(), [&groups, &descriptionsWithoutGroup, &descriptionsWithGroup](const auto& d) {
        if (d._group.size()) {
            groups.insert(d._group);
            descriptionsWithGroup.push_back(d);
        } else
            descriptionsWithoutGroup.push_back(d);
    });
    auto groupRow = 3;
    // add buttons in groups
    auto topLevelGroup = std::make_shared<Group>();
    for (const auto& description : descriptionsWithGroup) {
        const auto groups = SplitString(description._group, '/');
        Group* currentGroup = topLevelGroup.get();
        for (std::size_t groupIndex = 0; groupIndex < groups.size(); ++groupIndex) {
            const auto& group = groups[groupIndex];
            const auto iter = std::find_if(currentGroup->_children.begin(),
                                           currentGroup->_children.end(),
                                           [&group](const std::shared_ptr<Group>& grp) -> bool { return grp->_name == group; });
            if (iter == currentGroup->_children.end()) {
                auto newGroup = std::make_shared<Group>();
                newGroup->_name = group;
                newGroup->_parent = currentGroup;
                currentGroup->_children.push_back(newGroup);
                currentGroup = newGroup.get();
            } else
                currentGroup = iter->get();
            if (groupIndex == groups.size() - 1)
                currentGroup->_descriptions.push_back(description);
        }
    }
    std::function<void(Wt::WGridLayout*, int&, const std::shared_ptr<Group>&)> addGroup =
        [&](Wt::WGridLayout* layout, int& row, const std::shared_ptr<Group>& group) -> void {
        auto groupGroupBox = layout->addWidget(std::make_unique<WGroupBox>(group->_name), row++, 0, 1, 5, AlignmentFlag::Top);
        auto groupLayout = groupGroupBox->setLayout(std::make_unique<WGridLayout>());

        int subRow = 0;
        int subColumn = 0;
        std::sort(group->_children.begin(), group->_children.end(), [](const auto& a, const auto& b) { return a->_name.compare(b->_name) < 0; });
        for (const auto& subGroup : group->_children)
            addGroup(groupLayout, subRow, subGroup);
        std::sort(
            group->_descriptions.begin(), group->_descriptions.end(), [](const auto& a, const auto& b) { return a._name.compare(b._name) < 0; });
        for (auto& description : group->_descriptions) {
            auto button = AddDeviceButton(groupLayout, description, subRow, subColumn);
            _deviceWidgets.insert(std::make_pair(button, groupLayout));
        }
    };
    std::sort(topLevelGroup->_children.begin(), topLevelGroup->_children.end(), [](const auto& a, const auto& b) {
        return a->_name.compare(b->_name) < 0;
    });
    for (const auto& group : topLevelGroup->_children)
        addGroup(_mainLayout, groupRow, group);
    // add buttons without groups
    auto buttonRow = groupRow;
    auto buttonColumn = 0;
    std::sort(
        descriptionsWithoutGroup.begin(), descriptionsWithoutGroup.end(), [](const auto& a, const auto& b) { return a._name.compare(b._name) < 0; });
    for (const auto& description : descriptionsWithoutGroup) {
        auto button = AddDeviceButton(_mainLayout, description, buttonRow, buttonColumn);
        _deviceWidgets.insert(std::make_pair(button, _mainLayout));
    }
}

DeviceButton* DevicesWidget::AddDeviceButton(WGridLayout* layout, const ExtendedComponentDescription& description, int& row, int& column) {
    auto button =
        layout->addWidget(std::make_unique<DeviceButton>(_settings._servicePort, description), row, column, AlignmentFlag::Top | AlignmentFlag::Left);
    button->clicked().connect([this, description]() {
        if (description.isDeviceType()) {
            StackWidgetType stackWidgetType = static_cast<StackWidgetType>(-1);
            switch (description.getDeviceType()) {
                case DeviceType::Undefined:
                case DeviceType::Timer:
                    break;
                case DeviceType::Thermometer:
                    stackWidgetType = StackWidgetType::Thermometer;
                    break;
                case DeviceType::Relay:
                    stackWidgetType = StackWidgetType::Relay;
                    break;
                case DeviceType::MotionRelay:
                    stackWidgetType = StackWidgetType::MotionRelay;
                    break;
            }
            if (stackWidgetType != static_cast<StackWidgetType>(-1))
                _stackHolder->SetWidget(stackWidgetType, description._id.data());
        }
    });
    button->mouseWentUp().connect([this, description](const WMouseEvent& event) {
        if (event.button() == MouseButton::Right) {
            auto popup = std::make_unique<Wt::WPopupMenu>();
            auto deleteItem = popup->addItem("Удалить...");
            deleteItem->triggered().connect([this, &description]() {
                auto result = RequestHelper::DoDeleteRequest(
                    { BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE, "<string>", description._id.data()) },
                    Constants::LoginService,
                    {});
                if (result == 200)
                    Refresh();
                else
                    LOG_ERROR_MSG(fmt::format("Failed to delete device {}", description._id.data()));
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

void DevicesWidget::AddScenarioButton(WGridLayout* layout, const Scenario& scenario, int& row, int& column) {
    auto button = layout->addWidget(std::make_unique<WPushButton>(scenario._name), row, column, AlignmentFlag::Top | AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(button);
    button->clicked().connect([this, scenario]() {
        const auto result = RequestHelper::DoPatchRequest(
            { BACKEND_IP, _settings._servicePort, fmt::format("{}/{}", API_CLIENT_SCENARIOS, scenario._id.data()) }, Constants::LoginService, {});
        if (result != 200) {
            LOG_ERROR_MSG(fmt::format("Error while activating Scenario {}", scenario._name));
            WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Ошибка активации сценария!");
        } else
            WidgetHelper::ShowSimpleMessage(this, "Информация", "Сценарий активирован!", 5000);
    });
    ++column;
    if (column == 5) {
        ++row;
        column = 0;
    }
}
