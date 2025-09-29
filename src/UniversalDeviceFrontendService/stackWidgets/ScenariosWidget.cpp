#include "ScenariosWidget.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <set>
#include <vector>

#include <Wt/WCheckBox.h>
#include <Wt/WGlobal.h>
#include <Wt/WGroupBox.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "ApplicationSettings.hpp"
#include "Defines.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

ScenariosWidget::ScenariosWidget(IStackHolder* stackHolder, const ApplicationSettings& settings) :
    BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<WVBoxLayout>());

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<WHBoxLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto backButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, AlignmentFlag::Left | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto refreshButton = buttonsLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, AlignmentFlag::Right | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    WContainerWidget* editorCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1);
    auto editorLayout = editorCanvas->setLayout(std::make_unique<WHBoxLayout>());
    editorLayout->setContentsMargins(0, 0, 0, 0);

    WContainerWidget* listCanvas = editorLayout->addWidget(std::make_unique<WContainerWidget>(), 1);
    auto listLayout = listCanvas->setLayout(std::make_unique<WVBoxLayout>());
    listLayout->setContentsMargins(0, 0, 0, 0);

    auto deleteButton = listLayout->addWidget(std::make_unique<WPushButton>("Удалить"), 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(deleteButton);
    deleteButton->clicked().connect([this]() { DeleteScenario(); });

    _scenariosList = listLayout->addWidget(std::make_unique<WSelectionBox>(), 1);
    _scenariosList->setSelectionMode(SelectionMode::Single);
    _scenariosList->activated().connect([this](int index) { OnSelection(index); });

    WContainerWidget* selectionCanvas = editorLayout->addWidget(std::make_unique<WContainerWidget>(), 2);
    auto selectionLayout = selectionCanvas->setLayout(std::make_unique<WVBoxLayout>());
    selectionLayout->setContentsMargins(0, 0, 0, 0);

    WContainerWidget* addUpdateCanvas = selectionLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto addUpdateLayout = addUpdateCanvas->setLayout(std::make_unique<WHBoxLayout>());
    addUpdateLayout->setContentsMargins(0, 0, 0, 0);

    auto addButton = addUpdateLayout->addWidget(std::make_unique<WPushButton>("Добавить"), 0, AlignmentFlag::Left | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(addButton);
    addButton->clicked().connect([this]() { AddScenario(); });

    auto updateButton = addUpdateLayout->addWidget(std::make_unique<WPushButton>("Применить"), 0, AlignmentFlag::Right | AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(updateButton);
    updateButton->clicked().connect([this]() { UpdateScenario(); });

    selectionLayout->addWidget(std::make_unique<WText>("Имя:"), 0, AlignmentFlag::Top);
    _nameEditor = selectionLayout->addWidget(std::make_unique<WLineEdit>(), 0, AlignmentFlag::Top);

    WContainerWidget* groupsCanvas = selectionLayout->addWidget(std::make_unique<WContainerWidget>(), 1);
    auto groupsLayout = groupsCanvas->setLayout(std::make_unique<WHBoxLayout>());
    groupsLayout->setContentsMargins(0, 0, 0, 0);

    _activateEventsGroup = groupsLayout->addWidget(std::make_unique<WGroupBox>("Активировать:"), 1);

    _deactivateEventsGroup = groupsLayout->addWidget(std::make_unique<WGroupBox>("Деактивировать:"), 1);
}

void ScenariosWidget::Initialize(const std::string& data) { Refresh(); }

void ScenariosWidget::Clear() {
    _scenarios.clear();
    _scenariosList->clearSelection();
    _scenariosList->clear();
    _nameEditor->setText({});
    _events.clear();
    for (auto activateCheckBox : _activatedEvents)
        _activateEventsGroup->removeWidget(activateCheckBox);
    for (auto deactivateCheckBox : _deactivatedEvents)
        _deactivateEventsGroup->removeWidget(deactivateCheckBox);
    _activatedEvents.clear();
    _deactivatedEvents.clear();
}

void ScenariosWidget::Refresh() {
    Clear();

    const auto scenariosReplyJson =
        RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_SCENARIOS }, Constants::LoginService);
    _scenarios = !scenariosReplyJson.is_null() ? scenariosReplyJson.get<std::vector<Scenario>>() : std::vector<Scenario>{};

    const auto eventsReplyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService);
    for (const auto& eventJson : eventsReplyJson) {
        const Event& event = eventJson.get<Event>();
        _events.push_back(event);
    }

    for (const auto& scenario : _scenarios) {
        _scenariosList->addItem(scenario._name);
    }

    for (const auto& event : _events) {
        auto activateCheckBox = _activateEventsGroup->addWidget(std::make_unique<WCheckBox>(event._name));
        activateCheckBox->setInline(false);
        _activatedEvents.push_back(activateCheckBox);

        auto deactivateCheckBox = _deactivateEventsGroup->addWidget(std::make_unique<WCheckBox>(event._name));
        deactivateCheckBox->setInline(false);
        _deactivatedEvents.push_back(deactivateCheckBox);
    }
}

void ScenariosWidget::AddScenario() {
    if (IsUiValid() == false) {
        WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Неверно заполнены поля редактора сценариев!");
        return;
    }

    Scenario scenario{
        ._name = _nameEditor->text().toUTF8(),
        ._activateEvent = GetSelectedEventIndexes(_activatedEvents),
        ._deactivateEvent = GetSelectedEventIndexes(_deactivatedEvents),
    };
    const auto scenarioJson = static_cast<nlohmann::json>(scenario);
    const auto result =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_SCENARIOS }, Constants::LoginService, scenarioJson);
    if (result != 200) {
        LOG_ERROR_MSG(fmt::format("Error while adding new Scenario {}", scenarioJson.dump()));
        WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Ошибка добавления нового сценария!");
    }

    Refresh();
}

void ScenariosWidget::OnSelection(int index) {
    const int selectedIndex = _scenariosList->currentIndex();
    if (selectedIndex == -1)
        return;

    const Scenario& scenario = _scenarios.at(selectedIndex);

    _nameEditor->setText(scenario._name);
    std::set<std::size_t> activatedIndexes;
    std::set<std::size_t> deactivatedIndexes;
    for (std::size_t index = 0; index < _events.size(); ++index) {
        const Uuid& eventId = _events.at(index)._id;
        if (scenario._activateEvent.contains(eventId))
            activatedIndexes.insert(index);
        if (scenario._deactivateEvent.contains(eventId))
            deactivatedIndexes.insert(index);
    }
    SetSelectedEventIndexes(_activatedEvents, activatedIndexes);
    SetSelectedEventIndexes(_deactivatedEvents, deactivatedIndexes);
}

void ScenariosWidget::UpdateScenario() {
    const int selectedIndex = _scenariosList->currentIndex();
    if (selectedIndex == -1)
        return;

    if (IsUiValid() == false) {
        WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Неверно заполнены поля редактора сценариев!");
        return;
    }

    const Uuid scenarioId = _scenarios.at(selectedIndex)._id;

    Scenario scenario{
        ._id = scenarioId,
        ._name = _nameEditor->text().toUTF8(),
        ._activateEvent = GetSelectedEventIndexes(_activatedEvents),
        ._deactivateEvent = GetSelectedEventIndexes(_deactivatedEvents),
    };
    const auto scenarioJson = static_cast<nlohmann::json>(scenario);
    const auto result =
        RequestHelper::DoPutRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_SCENARIOS }, Constants::LoginService, scenarioJson);
    if (result != 200) {
        LOG_ERROR_MSG(fmt::format("Error while updating Scenario {}", scenarioJson.dump()));
        WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Ошибка обновления сценария!");
    }

    Refresh();
}

void ScenariosWidget::DeleteScenario() {
    const int selectedIndex = _scenariosList->currentIndex();
    if (selectedIndex == -1)
        return;

    const Uuid scenarioId = _scenarios.at(selectedIndex)._id;

    const auto result = RequestHelper::DoDeleteRequest(
        { BACKEND_IP, _settings._servicePort, fmt::format("{}/{}", std::string{ API_CLIENT_SCENARIOS }, scenarioId.data()) },
        Constants::LoginService);
    if (result != 200) {
        LOG_ERROR_MSG(fmt::format("Error while deleting Scenario {}", scenarioId.data()));
        WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Ошибка удаления сценария!");
    }

    Refresh();
}

std::set<Uuid> ScenariosWidget::GetSelectedEventIndexes(const std::vector<Wt::WCheckBox*>& container) const {
    std::set<Uuid> result;
    for (std::size_t index = 0; index < container.size(); ++index) {
        if (container.at(index)->isChecked())
            result.insert(_events[index]._id);
    }
    return result;
}

void ScenariosWidget::SetSelectedEventIndexes(const std::vector<Wt::WCheckBox*>& container, const std::set<std::size_t>& indexes) {
    for (std::size_t index = 0; index < container.size(); ++index) {
        container.at(index)->setChecked(indexes.contains(index));
    }
}

bool ScenariosWidget::IsUiValid() const {
    const auto selectedActivated = GetSelectedEventIndexes(_activatedEvents);
    const auto selectedDeactivated = GetSelectedEventIndexes(_deactivatedEvents);
    std::set<Uuid> intersection;
    std::set_intersection(selectedActivated.begin(),
                          selectedActivated.end(),
                          selectedDeactivated.begin(),
                          selectedDeactivated.end(),
                          std::inserter(intersection, intersection.begin()));

    return !_nameEditor->text().empty() && (selectedActivated.size() > 0 || selectedDeactivated.size() > 0) && intersection.size() == 0;
}
