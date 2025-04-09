#include "EventsWidget.hpp"

#include <memory>

#include <Wt/WGlobal.h>
#include <Wt/WGroupBox.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
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
    _mainLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());

    auto buttonsCanvas = _mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0, Wt::AlignmentFlag::Top);
    auto buttonsLayout = buttonsCanvas->setLayout(std::make_unique<Wt::WHBoxLayout>());
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    auto backButton = buttonsLayout->addWidget(std::make_unique<Wt::WPushButton>("Назад..."), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto refreshButton =
        buttonsLayout->addWidget(std::make_unique<Wt::WPushButton>("Обновить..."), 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });

    WContainerWidget* mainCanvas = _mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    auto mainLayout = mainCanvas->setLayout(std::make_unique<Wt::WHBoxLayout>());
    mainLayout->setContentsMargins(0, 0, 0, 0);

    WContainerWidget* tableCanvas = mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    auto tableLayout = tableCanvas->setLayout(std::make_unique<Wt::WVBoxLayout>());
    tableLayout->setContentsMargins(0, 0, 0, 0);

    auto deleteButton = tableLayout->addWidget(std::make_unique<Wt::WPushButton>("Удалить"), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(deleteButton);
    deleteButton->clicked().connect([&] { DeleteEvent(); });

    _eventsTableModel = std::make_shared<EventsTableModel>();

    _eventsTable = tableLayout->addWidget(std::make_unique<Wt::WTableView>(), 1, Wt::AlignmentFlag::Top);
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

    auto editorCanvas = mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 1);
    auto editLayout = editorCanvas->setLayout(std::make_unique<Wt::WVBoxLayout>());
    editLayout->setContentsMargins(0, 0, 0, 0);

    WContainerWidget* addUpdateCanvas = editLayout->addWidget(std::make_unique<WContainerWidget>(), 0, Wt::AlignmentFlag::Top);
    auto addUpdateLayout = addUpdateCanvas->setLayout(std::make_unique<Wt::WHBoxLayout>());
    addUpdateLayout->setContentsMargins(0, 0, 0, 0);

    auto addButton = addUpdateLayout->addWidget(std::make_unique<Wt::WPushButton>("Добавить"), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(addButton);
    addButton->clicked().connect([&] { AddEvent(); });

    auto updateButton =
        addUpdateLayout->addWidget(std::make_unique<Wt::WPushButton>("Применить"), 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Top);
    WidgetHelper::SetUsualButtonSize(updateButton);
    updateButton->clicked().connect([&] { UpdateEvent(); });

    auto eventGroup = editLayout->addWidget(std::make_unique<Wt::WGroupBox>("Событие"), 1, Wt::AlignmentFlag::Top);
    auto eventLayout = eventGroup->setLayout(std::make_unique<Wt::WVBoxLayout>());

    eventLayout->addWidget(std::make_unique<Wt::WText>("Тип события:"), 0, Wt::AlignmentFlag::Top);
    _eventType = eventLayout->addWidget(std::make_unique<Wt::WComboBox>(), 0, Wt::AlignmentFlag::Top);
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Timer));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Thermometer));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Relay));
    _eventType->addItem(EventsTableModel::EventTypeDisplayName(EventType::Thermostat));
    _eventType->changed().connect([this]() { OnEventTypeChanged(); });

    auto eventEditorCanvas = eventLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0, Wt::AlignmentFlag::Top);
    _eventEditorLayout = eventEditorCanvas->setLayout(std::make_unique<Wt::WVBoxLayout>());
    _eventEditorLayout->setContentsMargins(0, 0, 0, 0);

    Refresh();
}

void EventsWidget::Initialize(const std::string& data) { Refresh(); }

void EventsWidget::Clear() {
    _devices.clear();
    _eventsTableModel->UpdateData({});
    _eventType->setCurrentIndex(0);
    OnEventTypeChanged();
    _eventEditor->SetDevices(_devices);
    _eventEditor->Cleanup();
}

void EventsWidget::Refresh() {
    Clear();

    _devices = GetDevices();
    auto eventJsons = GetEvents();
    _eventsTableModel = std::make_shared<EventsTableModel>(eventJsons);
    _eventsTable->setModel(_eventsTableModel);
    _eventEditor->SetDevices(_devices);
}

std::vector<nlohmann::json> EventsWidget::GetEvents() {
    nlohmann::json result = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService);
    return result.is_null() ? std::vector<nlohmann::json>{} : result.get<std::vector<nlohmann::json>>();
}

std::vector<ExtendedComponentDescription> EventsWidget::GetDevices() {
    auto resultJson = RequestHelper::DoGetRequest({ BACKEND_IP, _settings._servicePort, API_CLIENT_DEVICES }, Constants::LoginService);
    std::vector<ExtendedComponentDescription> devices =
        resultJson.is_null() ? std::vector<ExtendedComponentDescription>{} : resultJson.get<std::vector<ExtendedComponentDescription>>();
    // right now not all devices can receive events
    auto newEnd = std::remove_if(devices.begin(), devices.end(), [](const ExtendedComponentDescription& device) {
        return !device.isDeviceType() || device.getDeviceType() == DeviceType::Undefined || device.getDeviceType() == DeviceType::UniversalDevice;
    });
    devices.erase(newEnd, devices.end());
    return devices;
}

void EventsWidget::AddEvent() {
    if (_eventType->currentIndex() < 0 || _eventEditor == nullptr || !_eventEditor->IsValid()) {
        ShowIncorrectEventMsgBox();
        return;
    }
    auto eventEditor = _eventEditor;
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
    if (GetSelectedEventIdFromTable().isEmpty() || _eventType->currentIndex() < 0 || _eventEditor == nullptr || !_eventEditor->IsValid()) {
        ShowIncorrectEventMsgBox();
        return;
    }
    auto eventEditor = _eventEditor;
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
        _eventEditor->FillUi(event);
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
    _eventEditorLayout->removeWidget(_eventEditor);
    _eventEditor = nullptr;
    switch (_eventType->currentIndex()) {
        case 0:
            _eventEditor = _eventEditorLayout->addWidget(std::make_unique<TimerEventEditor>(), 0, Wt::AlignmentFlag::Top);
            break;
        case 1:
            _eventEditor = _eventEditorLayout->addWidget(std::make_unique<ThermometerEventEditor>(), 0, Wt::AlignmentFlag::Top);
            break;
        case 2:
            _eventEditor = _eventEditorLayout->addWidget(std::make_unique<RelayEventEditor>(), 0, Wt::AlignmentFlag::Top);
            break;
        case 3:
            _eventEditor = _eventEditorLayout->addWidget(std::make_unique<ThermostatEventEditor>(), 0, Wt::AlignmentFlag::Top);
            break;
    }
    if (_eventEditor)
        _eventEditor->SetDevices(_devices);
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
