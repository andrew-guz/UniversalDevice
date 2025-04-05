#include "DevicesWidget.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include <Wt/Http/Cookie.h>
#include <Wt/WGlobal.h>
#include <Wt/WGroupBox.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WWidget.h>
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
#include "ScenarioButton.hpp"
#include "String.hpp"
#include "UrlHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

#define MAX_BUTTONS_IN_A_ROW 5

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
    _mainLayout = setLayout(std::make_unique<WVBoxLayout>());

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WHBoxLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto exitButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Выход"), 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(exitButton);
    exitButton->clicked().connect([this]() {
        WApplication::instance()->removeCookie(Http::Cookie{ "authorization" });
        _stackHolder->SetWidget(StackWidgetType::Login, {});
    });

    auto eventsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("События"), 0, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(eventsButton);
    eventsButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Events, {}); });

    auto scenariosButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Сценарии"), 0, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(scenariosButton);
    scenariosButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Scenarios, {}); });

    auto logsButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Логи"), 0, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(logsButton);
    logsButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Logs, {}); });

    auto refreshButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    Refresh();
}

void DevicesWidget::Initialize(const std::string& data) { Refresh(); }

void DevicesWidget::Clear() {
    for (auto iter = _widgets.rbegin(); iter != _widgets.rend(); ++iter) {
        iter->second->removeWidget(iter->first);
    }

    _widgets.clear();
}

void DevicesWidget::Refresh() {
    Clear();

    WContainerWidget* buttonsWidget = nullptr;
    WHBoxLayout* buttonsLayout = nullptr;

    const auto scenariosReplyJson =
        RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_SCENARIOS }, Constants::LoginService);
    const auto scenarios = !scenariosReplyJson.is_null() ? scenariosReplyJson.get<std::vector<Scenario>>() : std::vector<Scenario>{};

    const auto devicesReplyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_DEVICES }, Constants::LoginService);
    const auto allDescriptions =
        !devicesReplyJson.is_null() ? devicesReplyJson.get<std::vector<ExtendedComponentDescription>>() : std::vector<ExtendedComponentDescription>{};
    if (allDescriptions.empty())
        return;
    LOG_DEBUG_MSG(fmt::format("{} descriptions found", allDescriptions.size()));

    auto initButtonsWidget = [&buttonsWidget, &buttonsLayout, this](WVBoxLayout* layout) {
        buttonsWidget = layout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
        _widgets.push_back(std::make_pair<WWidget*, WLayout*>(buttonsWidget, layout));
        buttonsLayout = buttonsWidget->setLayout(std::make_unique<WHBoxLayout>());
        buttonsLayout->setContentsMargins(0, 0, 0, 0);
    };

    auto clearButtonsWidget = [&buttonsWidget, &buttonsLayout]() {
        buttonsWidget = nullptr;
        buttonsLayout = nullptr;
    };

    if (scenarios.size()) {
        auto scenariosCanvas = _mainLayout->addWidget(std::make_unique<WGroupBox>("Сценарии"), 0, AlignmentFlag::Top);
        _widgets.push_back(std::make_pair<WWidget*, WLayout*>(scenariosCanvas, _mainLayout));
        auto scenariosLayout = scenariosCanvas->setLayout(std::make_unique<WVBoxLayout>());
        scenariosLayout->setContentsMargins(0, 0, 0, 0);

        for (std::size_t i = 0; i < scenarios.size(); ++i) {
            if (buttonsWidget == nullptr) {
                initButtonsWidget(scenariosLayout);
            }

            AddScenarioButton(buttonsLayout, scenarios.at(i));

            if (i % MAX_BUTTONS_IN_A_ROW == MAX_BUTTONS_IN_A_ROW - 1) {
                clearButtonsWidget();
            }
        }

        clearButtonsWidget();
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
    std::function<void(Wt::WVBoxLayout*, const std::shared_ptr<Group>&)> addGroup = [&](Wt::WVBoxLayout* layout,
                                                                                        const std::shared_ptr<Group>& group) -> void {
        auto groupGroupBox = layout->addWidget(std::make_unique<WGroupBox>(group->_name), 0, AlignmentFlag::Top);
        _widgets.push_back(std::make_pair<WWidget*, WLayout*>(groupGroupBox, layout));
        auto groupLayout = groupGroupBox->setLayout(std::make_unique<WVBoxLayout>());
        groupLayout->setContentsMargins(0, 0, 0, 0);

        std::sort(group->_children.begin(), group->_children.end(), [](const auto& a, const auto& b) { return a->_name.compare(b->_name) < 0; });
        for (const auto& subGroup : group->_children)
            addGroup(groupLayout, subGroup);

        // add buttons in group
        std::sort(
            group->_descriptions.begin(), group->_descriptions.end(), [](const auto& a, const auto& b) { return a._name.compare(b._name) < 0; });

        for (std::size_t i = 0; i < group->_descriptions.size(); ++i) {
            if (buttonsWidget == nullptr) {
                initButtonsWidget(groupLayout);
            }

            AddDeviceButton(buttonsLayout, group->_descriptions.at(i));

            if (i % MAX_BUTTONS_IN_A_ROW == MAX_BUTTONS_IN_A_ROW - 1) {
                clearButtonsWidget();
            }
        }

        clearButtonsWidget();
    };
    std::sort(topLevelGroup->_children.begin(), topLevelGroup->_children.end(), [](const auto& a, const auto& b) {
        return a->_name.compare(b->_name) < 0;
    });
    for (const auto& group : topLevelGroup->_children)
        addGroup(_mainLayout, group);

    // add groups without layout
    for (std::size_t i = 0; i < descriptionsWithoutGroup.size(); ++i) {
        if (buttonsWidget == nullptr) {
            initButtonsWidget(_mainLayout);
            buttonsLayout->setContentsMargins(0, 0, 0, 0);
        }

        AddDeviceButton(buttonsLayout, descriptionsWithoutGroup.at(i));

        if (i % MAX_BUTTONS_IN_A_ROW == MAX_BUTTONS_IN_A_ROW - 1) {
            clearButtonsWidget();
        }
    }

    clearButtonsWidget();

    auto expander = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 10, AlignmentFlag::Bottom);
    _widgets.push_back(std::make_pair<WWidget*, WLayout*>(expander, _mainLayout));
}

void DevicesWidget::AddDeviceButton(WHBoxLayout* layout, const ExtendedComponentDescription& description) {
    DeviceButton* button =
        layout->addWidget(std::make_unique<DeviceButton>(_settings._servicePort, description), 0, AlignmentFlag::Top | AlignmentFlag::Left);
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
                case DeviceType::UniversalDevice:
                    stackWidgetType = StackWidgetType::UniversalDevice;
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
                    { BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE, "<string>", description._id.data()) }, Constants::LoginService);
                if (result == 200)
                    Refresh();
                else
                    LOG_ERROR_MSG(fmt::format("Failed to delete device {}", description._id.data()));
            });
            popup->exec(event);
        }
    });
    _widgets.push_back(std::make_pair<WWidget*, WLayout*>(button, layout));
}

void DevicesWidget::AddScenarioButton(WHBoxLayout* layout, const Scenario& scenario) {
    ScenarioButton* button =
        layout->addWidget(std::make_unique<ScenarioButton>(_settings._servicePort, scenario), 0, AlignmentFlag::Top | AlignmentFlag::Left);
    _widgets.push_back(std::make_pair<WWidget*, WLayout*>(button, layout));
}
