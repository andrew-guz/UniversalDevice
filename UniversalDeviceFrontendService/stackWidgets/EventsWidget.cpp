#include "EventsWidget.hpp"

#include <Wt/WGroupBox.h>
#include <Wt/WText.h>

#include "Constants.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "MessageHelper.hpp"
#include "RelayEvent.hpp"
#include "RelayEventEditor.hpp"
#include "RelayState.hpp"
#include "RequestHelper.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerEventEditor.hpp"
#include "ThermometerLedBrightness.hpp"
#include "ThermostatEvent.hpp"
#include "ThermostatEventEditor.hpp"
#include "TimerEvent.hpp"
#include "TimerEventEditor.hpp"
#include "UrlHelper.hpp"
#include "WidgetHelper.hpp"
#include "Wt/WGlobal.h"

using namespace Wt;

EventsWidget::EventsWidget(IStackHolder* stackHolder, const Settings& settings) : BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto refreshButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    auto tableCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 0);
    auto tableLayout = tableCanvas->setLayout(std::make_unique<WGridLayout>());

    _deleteButton = tableLayout->addWidget(std::make_unique<WPushButton>("Удалить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(_deleteButton);
    _deleteButton->clicked().connect([&] { DeleteEvent(); });

    _eventsTableModel = std::make_shared<EventsTableModel>();

    _eventsTable = tableLayout->addWidget(std::make_unique<WTableView>(), 1, 0, AlignmentFlag::Top);
    _eventsTable->setColumnResizeEnabled(false);
    _eventsTable->setColumnAlignment(0, AlignmentFlag::Center);
    _eventsTable->setColumnAlignment(1, AlignmentFlag::Center);
    _eventsTable->setAlternatingRowColors(true);
    _eventsTable->setSelectionMode(SelectionMode::Single);
    _eventsTable->setSelectionBehavior(SelectionBehavior::Rows);
    _eventsTable->setEditTriggers(EditTrigger::None);
    _eventsTable->setModel(_eventsTableModel);
    _eventsTable->setColumnWidth(3, 250);
    _eventsTable->selectionChanged().connect([this]() { OnTableSelectionChanged(); });

    tableLayout->setRowStretch(0, 0);
    tableLayout->setRowStretch(1, 1);

    auto editorCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 1);
    auto editLayout = editorCanvas->setLayout(std::make_unique<WGridLayout>());

    _addButton = editLayout->addWidget(std::make_unique<WPushButton>("Добавить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(_addButton);
    _addButton->clicked().connect([&] { AddEvent(); });

    _updateButton = editLayout->addWidget(std::make_unique<WPushButton>("Применить"), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(_updateButton);
    _updateButton->clicked().connect([&] { UpdateEvent(); });

    auto eventGroup = editLayout->addWidget(std::make_unique<WGroupBox>("Событие"), 1, 0, 1, 2, AlignmentFlag::Top);
    auto eventLayout = eventGroup->setLayout(std::make_unique<WGridLayout>());

    eventLayout->addWidget(std::make_unique<WText>("Тип события:"), 0, 0, 1, 2);
    _eventType = eventLayout->addWidget(std::make_unique<WComboBox>(), 1, 0, 1, 2);
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(Constants::EventTypeTimer));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(Constants::EventTypeThermometer));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(Constants::EventTypeRelay));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(Constants::EventTypeThermostat));
    _eventType->changed().connect([this]() { OnEventTypeChanged(); });

    _eventEditorsStack = eventLayout->addWidget(std::make_unique<WStackedWidget>(), 2, 0, 1, 2);
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
            LOG_ERROR << "Error while adding new Event " << eventJson.dump() << "." << std::endl;
    }
    Refresh();
}

void EventsWidget::DeleteEvent() {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    auto result = RequestHelper::DoDeleteRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
    if (result != 200)
        LOG_ERROR << "Error while deleting Event " << eventJson.dump() << "." << std::endl;
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
            LOG_ERROR << "Error while updating Event " << eventJson.dump() << "." << std::endl;
    }
    Refresh();
}

void EventsWidget::OnTableSelectionChanged() {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    auto simpleEvent = eventJson.get<Event>();
    if (simpleEvent._type == Constants::EventTypeTimer) {
        _eventType->setCurrentIndex(0);
        OnEventTypeChanged();
        GetCurrentEventEditor()->FillUi(eventJson.get<TimerEvent>());
    } else if (simpleEvent._type == Constants::EventTypeThermometer) {
        _eventType->setCurrentIndex(1);
        OnEventTypeChanged();
        GetCurrentEventEditor()->FillUi(eventJson.get<ThermometerEvent>());
    } else if (simpleEvent._type == Constants::EventTypeRelay) {
        _eventType->setCurrentIndex(2);
        OnEventTypeChanged();
        GetCurrentEventEditor()->FillUi(eventJson.get<RelayEvent>());
    } else if (simpleEvent._type == Constants::EventTypeThermostat) {
        _eventType->setCurrentIndex(3);
        OnEventTypeChanged();
        GetCurrentEventEditor()->FillUi(eventJson.get<ThermostatEvent>());
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
    return cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
}

Uuid EventsWidget::GetSelectedEventIdFromTable() const {
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return Uuid::Empty();
    auto selectedEventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    return selectedEventJson.get<Event>()._id;
}

void EventsWidget::ShowIncorrectEventMsgBox() { WidgetHelper::ShowSimpleErrorMessage(this, "Ошибка", "Неверно заполнены поля редактора событий!"); }
