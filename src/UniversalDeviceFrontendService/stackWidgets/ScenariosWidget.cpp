#include "ScenariosWidget.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <set>
#include <vector>

#include "fmt/format.h"
#include "nlohmann/json_fwd.hpp"
#include <Wt/WCheckBox.h>
#include <Wt/WGlobal.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WText.h>

#include "Defines.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

ScenariosWidget::ScenariosWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto refreshButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    auto scenariosCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 0, AlignmentFlag::Top);
    scenariosCanvas->setStyleClass("background-color: red;");
    auto scenariosLayout = scenariosCanvas->setLayout(std::make_unique<WGridLayout>());

    auto deleteButton = scenariosLayout->addWidget(std::make_unique<WPushButton>("Удалить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(deleteButton);
    deleteButton->clicked().connect([this]() { DeleteScenario(); });

    _scenariosList = scenariosLayout->addWidget(std::make_unique<WSelectionBox>(), 1, 0);
    _scenariosList->setSelectionMode(SelectionMode::Single);
    _scenariosList->activated().connect([this](int index) { OnSelection(index); });

    auto editCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 1, AlignmentFlag::Top);
    auto editLayout = editCanvas->setLayout(std::make_unique<WGridLayout>());

    auto addButton = editLayout->addWidget(std::make_unique<WPushButton>("Добавить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(addButton);
    addButton->clicked().connect([this]() { AddScenario(); });

    auto updateButton = editLayout->addWidget(std::make_unique<WPushButton>("Обновить"), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(updateButton);
    updateButton->clicked().connect([this]() { UpdateScenario(); });

    auto scenarioGroup = editLayout->addWidget(std::make_unique<WGroupBox>("Событие"), 1, 0, 1, 2);
    auto scenarioLayout = scenarioGroup->setLayout(std::make_unique<WGridLayout>());

    scenarioLayout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    _nameEditor = scenarioLayout->addWidget(std::make_unique<WLineEdit>(), 1, 0, 1, 2);

    auto activateEventsGroup = scenarioLayout->addWidget(std::make_unique<WGroupBox>("Активировать:"), 2, 0);
    _activateEventsLayout = activateEventsGroup->setLayout(std::make_unique<WGridLayout>());

    auto deactivateEventsGroup = scenarioLayout->addWidget(std::make_unique<WGroupBox>("Деактивировать:"), 2, 1);
    _deactivateEventsLayout = deactivateEventsGroup->setLayout(std::make_unique<WGridLayout>());
}

void ScenariosWidget::Initialize(const std::string& data) { Refresh(); }

void ScenariosWidget::Clear() {
    _scenarios.clear();
    _scenariosList->clearSelection();
    _scenariosList->clear();
    _nameEditor->setText({});
    _events.clear();
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

    int eventRow = 0;
    for (const auto& event : _events) {
        _activatedEvents.push_back(_activateEventsLayout->addWidget(std::make_unique<WCheckBox>(event._name), eventRow, 0));
        _deactivatedEvents.push_back(_deactivateEventsLayout->addWidget(std::make_unique<WCheckBox>(event._name), eventRow, 0));
        ++eventRow;
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
        Constants::LoginService,
        {});
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
