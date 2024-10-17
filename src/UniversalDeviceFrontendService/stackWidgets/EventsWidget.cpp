#include "EventsWidget.hpp"

#include <Wt/WGlobal.h>
#include <Wt/WGroupBox.h>
#include <Wt/WText.h>
#include <fmt/format.h>

#include "Constants.hpp"
#include "Defines.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayEvent.hpp"
#include "RelayEventEditor.hpp"
#include "RequestHelper.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerEventEditor.hpp"
#include "ThermostatEvent.hpp"
#include "ThermostatEventEditor.hpp"
#include "TimerEvent.hpp"
#include "TimerEventEditor.hpp"
#include "WidgetHelper.hpp"

EventsWidget::EventsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<Wt::WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<Wt::WPushButton>("Назад..."), 0, 0, Wt::AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto refreshButton = _mainLayout->addWidget(std::make_unique<Wt::WPushButton>("Обновить..."), 0, 1, Wt::AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    auto tableCanvas = _mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1, 0);
    auto tableLayout = tableCanvas->setLayout(std::make_unique<Wt::WGridLayout>());

    auto deleteButton = tableLayout->addWidget(std::make_unique<Wt::WPushButton>("Удалить"), 0, 0, Wt::AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(deleteButton);
    deleteButton->clicked().connect([&] { DeleteEvent(); });

    _eventsTableModel = std::make_shared<EventsTableModel>();

    _eventsTable = tableLayout->addWidget(std::make_unique<Wt::WTableView>(), 1, 0, Wt::AlignmentFlag::Top);
    _eventsTable->setColumnResizeEnabled(false);
    _eventsTable->setColumnAlignment(0, Wt::AlignmentFlag::Center);
    _eventsTable->setColumnAlignment(1, Wt::AlignmentFlag::Center);
    _eventsTable->setAlternatingRowColors(true);
    _eventsTable->setSelectionMode(Wt::SelectionMode::Single);
    _eventsTable->setSelectionBehavior(Wt::SelectionBehavior::Rows);
    _eventsTable->setEditTriggers(Wt::EditTrigger::None);
    _eventsTable->setModel(_eventsTableModel);
    _eventsTable->setColumnWidth(3, 250);
    _eventsTable->selectionChanged().connect([this]() { OnTableSelectionChanged(); });

    tableLayout->setRowStretch(0, 0);
    tableLayout->setRowStretch(1, 1);

    auto editorCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 1);
    auto editLayout = editorCanvas->setLayout(std::make_unique<Wt::WGridLayout>());

    auto addButton = editLayout->addWidget(std::make_unique<Wt::WPushButton>("Добавить"), 0, 0, Wt::AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(addButton);
    addButton->clicked().connect([&] { AddEvent(); });

    auto updateButton = editLayout->addWidget(std::make_unique<Wt::WPushButton>("Применить"), 0, 1, Wt::AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(updateButton);
    updateButton->clicked().connect([&] { UpdateEvent(); });

    auto eventGroup = editLayout->addWidget(std::make_unique<Wt::WGroupBox>("Событие"), 1, 0, 1, 2, Wt::AlignmentFlag::Top);
    auto eventLayout = eventGroup->setLayout(std::make_unique<Wt::WGridLayout>());

    eventLayout->addWidget(std::make_unique<Wt::WText>("Тип события:"), 0, 0, 1, 2);
    _eventType = eventLayout->addWidget(std::make_unique<Wt::WComboBox>(), 1, 0, 1, 2);
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Timer));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Thermometer));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Relay));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Thermostat));
    _eventType->changed().connect([this]() { OnEventTypeChanged(); });

    _eventEditorsStack = eventLayout->addWidget(std::make_unique<Wt::WStackedWidget>(), 2, 0, 1, 2);
    _eventEditorsStack->addWidget(std::make_unique<TimerEventEditor>());
    _eventEditorsStack->addWidget(std::make_unique<ThermometerEventEditor>());
    _eventEditorsStack->addWidget(std::make_unique<RelayEventEditor>());
    _eventEditorsStack->addWidget(std::make_unique<ThermostatEventEditor>());

    editLayout->setRowStretch(0, 0);
    editLayout->setRowStretch(1, 1);

    _mainLayout->setRowStretch(0, 0);
    _mainLayout->setRowStretch(1, 1);

    Refresh();
}

void EventsWidget::Initialize(const std::string& data) { Refresh(); }

void EventsWidget::Clear() {
    _devices.clear();
    _eventsTableModel->UpdateData({});
    _eventType->setCurrentIndex(0);
    OnEventTypeChanged();
    for (auto i = 0; i < _eventEditorsStack->count(); ++i) {
        auto editor = dynamic_cast<IEventEditorWidget*>(_eventEditorsStack->widget(i));
        editor->SetDevices(_devices);
        editor->Cleanup();
    }
}

void EventsWidget::Refresh() {
    Clear();

    _devices = GetDevices();
    auto eventJsons = GetEvents();
    _eventsTableModel = std::make_shared<EventsTableModel>(eventJsons);
    _eventsTable->setModel(_eventsTableModel);
    for (auto i = 0; i < _eventEditorsStack->count(); ++i) {
        auto editor = dynamic_cast<IEventEditorWidget*>(_eventEditorsStack->widget(i));
        editor->SetDevices(_devices);
    }
}

std::vector<nlohmann::json> EventsWidget::GetEvents() {
    return RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService);
}

std::vector<ExtendedComponentDescription> EventsWidget::GetDevices() {
    auto resultJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_DEVICES }, Constants::LoginService);
    return resultJson.get<std::vector<ExtendedComponentDescription>>();
}

void EventsWidget::AddEvent() {
    if (_eventType->currentIndex() < 0 || GetCurrentEventEditor() == nullptr || !GetCurrentEventEditor()->IsValid()) {
        ShowIncorrectEventMsgBox();
        return;
    }
    auto eventEditor = GetCurrentEventEditor();
    nlohmann::json eventJson = CreateNewEventFromEditor(eventEditor);
    if (!eventJson.is_null()) {
        auto result = RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
        if (result != 200)
            LOG_ERROR_MSG(fmt::format("Error while adding new Event {}", eventJson.dump()));
    }
    Refresh();
}

void EventsWidget::DeleteEvent() {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = Wt::cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), Wt::ItemDataRole::User));
    auto result = RequestHelper::DoDeleteRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
    if (result != 200)
        LOG_ERROR_MSG(fmt::format("Error while deleting Event {}", eventJson.dump()));
    Refresh();
}

void EventsWidget::UpdateEvent() {
    if (GetSelectedEventIdFromTable().isEmpty() || _eventType->currentIndex() < 0 || GetCurrentEventEditor() == nullptr ||
        !GetCurrentEventEditor()->IsValid()) {
        ShowIncorrectEventMsgBox();
        return;
    }
    auto eventEditor = GetCurrentEventEditor();
    auto eventJson = CreateNewEventFromEditor(eventEditor);
    eventJson["id"] = GetSelectedEventIdFromTable().data();
    if (!eventJson.is_null()) {
        auto result = RequestHelper::DoPutRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
        if (result != 200)
            LOG_ERROR_MSG(fmt::format("Error while updating Event {}", eventJson.dump()));
    }
    Refresh();
}

void EventsWidget::OnTableSelectionChanged() {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = Wt::cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), Wt::ItemDataRole::User));
    auto simpleEvent = eventJson.get<Event>();
    const auto applyType = [&](int index, const Event& event) -> void {
        _eventType->setCurrentIndex(index);
        OnEventTypeChanged();
        GetCurrentEventEditor()->FillUi(event);
    };
    switch (simpleEvent._type) {
        case EventType::Undefined:
            LOG_ERROR_MSG("Invalid event type");
            break;
        case EventType::Timer:
            applyType(0, eventJson.get<TimerEvent>());
            break;
        case EventType::Thermometer:
            applyType(1, eventJson.get<ThermometerEvent>());
            break;
        case EventType::Relay:
            applyType(2, eventJson.get<RelayEvent>());
            break;
        case EventType::Thermostat:
            applyType(3, eventJson.get<ThermostatEvent>());
            break;
    }
}

void EventsWidget::OnEventTypeChanged() {
    _eventEditorsStack->setCurrentIndex(_eventType->currentIndex());
    for (auto i = 0; i < 4; ++i) {
        if (i == _eventEditorsStack->currentIndex())
            continue;
        dynamic_cast<BaseEventEditor*>(_eventEditorsStack->widget(i))->Cleanup();
    }
}

BaseEventEditor* EventsWidget::GetCurrentEventEditor() const {
    if (_eventEditorsStack->currentIndex() < 0)
        return nullptr;
    return dynamic_cast<BaseEventEditor*>(_eventEditorsStack->currentWidget());
}

nlohmann::json EventsWidget::CreateNewEventFromEditor(BaseEventEditor* eventEditor) const {
    nlohmann::json eventJson;
    switch (_eventType->currentIndex()) {
        case 0: {
            TimerEvent event;
            eventEditor->FillFromUi(event);
            eventJson = event;
        } break;
        case 1: {
            ThermometerEvent event;
            eventEditor->FillFromUi(event);
            eventJson = event;
        } break;
        case 2: {
            RelayEvent event;
            eventEditor->FillFromUi(event);
            eventJson = event;
        } break;
        case 3: {
            ThermostatEvent event;
            eventEditor->FillFromUi(event);
            eventJson = event;
        } break;
    }
    return eventJson;
}

nlohmann::json EventsWidget::GetSelectedEventJsonFromTable() const {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return {};
    return Wt::cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), Wt::ItemDataRole::User));
}

Uuid EventsWidget::GetSelectedEventIdFromTable() const {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return Uuid::Empty();
    auto selectedEventJson = Wt::cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), Wt::ItemDataRole::User));
    return selectedEventJson.get<Event>()._id;
}

void EventsWidget::ShowIncorrectEventMsgBox() { WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Неверно заполнены поля редактора событий!"); }
