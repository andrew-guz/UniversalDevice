#include "EventsWidget.h"

#include <Wt/WText.h>
#include <Wt/WGroupBox.h>

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "MessageHelper.h"
#include "UrlHelper.h"
#include "RequestHelper.h"
#include "WidgetHelper.h"
#include "TimerEvent.h"
#include "ThermometerEvent.h"
#include "RelayEvent.h"
#include "ThermostatEvent.h"
#include "ThermometerLedBrightness.h"
#include "RelayState.h"
#include "TimerEventEditor.h"
#include "ThermometerEventEditor.h"
#include "ThermostatEventEditor.h"
#include "RelayEventEditor.h"

using namespace Wt;

EventsWidget::EventsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Devices, "");
    });

    auto refreshButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Обновить..."), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([&](){
        Refresh();
    });

    auto tableCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 0);
    auto tableLayout = tableCanvas->setLayout(std::make_unique<WGridLayout>());

    _deleteButton = tableLayout->addWidget(std::make_unique<WPushButton>("Удалить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(_deleteButton);
    _deleteButton->clicked().connect([&]{
        DeleteEvent();
    });

    _eventsTableModel = std::make_shared<EventsTableModel>();

    _eventsTable = tableLayout->addWidget(std::make_unique<WTableView>(), 1, 0);
    _eventsTable->setColumnResizeEnabled(false);
    _eventsTable->setColumnAlignment(0, AlignmentFlag::Center);
    _eventsTable->setColumnAlignment(1, AlignmentFlag::Center);
    _eventsTable->setAlternatingRowColors(true);
    _eventsTable->setSelectionMode(SelectionMode::Single);
    _eventsTable->setSelectionBehavior(SelectionBehavior::Rows);
    _eventsTable->setEditTriggers(EditTrigger::None);
    _eventsTable->setModel(_eventsTableModel);
    _eventsTable->selectionChanged().connect([&]() {
        OnTableSelectionChanged();
    });

    auto editorCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 1);
    auto editLayout = editorCanvas->setLayout(std::make_unique<WGridLayout>());

    _addButton = editLayout->addWidget(std::make_unique<WPushButton>("Добавить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(_addButton);
    _addButton->clicked().connect([&]{
        AddEvent();
    });

    _updateButton = editLayout->addWidget(std::make_unique<WPushButton>("Применить"), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(_updateButton);
    _updateButton->clicked().connect([&]{
        UpdateEvent();
    });

    auto eventGroup = editLayout->addWidget(std::make_unique<WGroupBox>("Событие"), 1, 0, 1, 2);
    auto eventLayout = eventGroup->setLayout(std::make_unique<WGridLayout>());

    eventLayout->addWidget(std::make_unique<WText>("Тип события:"), 0, 0, 1, 2);
    _eventType = eventLayout->addWidget(std::make_unique<WComboBox>(), 1, 0, 1, 2);
    _eventType->addItem("Таймер");      //Constants::EventTypeTimer
    _eventType->addItem("Термометр");   //Constants::EventTypeThermometer
    _eventType->addItem("Рэле");        //Constants::EventTypeRelay
    _eventType->addItem("Термостат");   //Constants::EventTypeThermostat
    _eventType->changed().connect([&](){
        OnEventTypeChanged();
    });
    
    _eventEditorsStack = eventLayout->addWidget(std::make_unique<WStackedWidget>(), 2, 0, 1, 2);
    _eventEditorsStack->addWidget(std::make_unique<TimerEventEditor>());
    _eventEditorsStack->addWidget(std::make_unique<ThermometerEventEditor>());
    _eventEditorsStack->addWidget(std::make_unique<RelayEventEditor>());
    _eventEditorsStack->addWidget(std::make_unique<ThermostatEventEditor>());

    _eventsTable->setMinimumSize(200, 500);

    Refresh();
}

void EventsWidget::Initialize(const std::string& data)
{
    Refresh();
}

void EventsWidget::Clear()
{
    _devices.clear();
    _eventsTableModel->UpdateData({});
    _eventType->setCurrentIndex(0);
    OnEventTypeChanged();
    for (auto i = 0; i < _eventEditorsStack->count(); ++i)
    {
        auto editor = dynamic_cast<IEventEditorWidget*>(_eventEditorsStack->widget(i));
        editor->SetDevices(_devices);
        editor->Cleanup();
    }
}

void EventsWidget::Refresh()
{
    Clear();

    _devices = GetDevices();
    auto eventJsons = GetEvents();
    _eventsTableModel = std::make_shared<EventsTableModel>(eventJsons);
    _eventsTable->setModel(_eventsTableModel);
    for (auto i = 0; i < _eventEditorsStack->count(); ++i)
    {
        auto editor = dynamic_cast<IEventEditorWidget*>(_eventEditorsStack->widget(i));
        editor->SetDevices(_devices);
    }
}

std::vector<nlohmann::json> EventsWidget::GetEvents()
{
    return RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService);
}

std::vector<ExtendedComponentDescription> EventsWidget::GetDevices()
{
    auto resultJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICES }, Constants::LoginService);
    return JsonExtension::CreateVectorFromJson<ExtendedComponentDescription>(resultJson);
}

void EventsWidget::AddEvent()
{
    /*if (_devices.empty())
        return;
    size_t providerIndex = _providers->currentIndex();
    size_t receiverIndex = _receivers->currentIndex();
    if (providerIndex < 0 ||
        providerIndex >= _devices.size() + 1 ||
        receiverIndex < 0 ||
        receiverIndex >= _devices.size())
        return;
    if (_name->text().empty())
        return;
    nlohmann::json eventJson;
    if (providerIndex == 0)
        eventJson = GetTimerEventFromUi({}, providerIndex, receiverIndex);
    else
    {
        auto providerDevice = _devices[providerIndex - 1];
        if (providerDevice._type == Constants::DeviceTypeThermometer)
            eventJson = GetThermometerEventFromUi({}, providerIndex, receiverIndex);
        if (providerDevice._type == Constants::DeviceTypeRelay)
            eventJson = GetRelayEventFromUi({}, providerIndex, receiverIndex);
    }
    if (!eventJson.is_null())
    {
        auto result = RequestHelper::DoPostRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
        if (result != 200)
            LOG_ERROR << "Error while adding new Event " << eventJson.dump() << "." << std::endl;
    }
    Refresh();*/
}

void EventsWidget::DeleteEvent()
{
    /*auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    auto result = RequestHelper::DoDeleteRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
    if (result != 200)
            LOG_ERROR << "Error while deleting Event " << eventJson.dump() << "." << std::endl;
    Refresh();*/
}

void EventsWidget::UpdateEvent()
{
    /*auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    if (_devices.empty())
        return;
    size_t providerIndex = _providers->currentIndex();
    size_t receiverIndex = _receivers->currentIndex();
    if (providerIndex < 0 ||
        providerIndex >= _devices.size() + 1 ||
        receiverIndex < 0 ||
        receiverIndex >= _devices.size())
        return;
    if (_name->text().empty())
        return;
    auto selectedEventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    auto selectedEventId = JsonExtension::CreateFromJson<Event>(selectedEventJson)._id;
    nlohmann::json eventJson;
    if (providerIndex == 0)
        eventJson = GetTimerEventFromUi(selectedEventId, providerIndex, receiverIndex);
    else
    {
        auto providerDevice = _devices[providerIndex - 1];
        if (providerDevice._type == Constants::DeviceTypeThermometer)
            eventJson = GetThermometerEventFromUi(selectedEventId, providerIndex, receiverIndex);
        if (providerDevice._type == Constants::DeviceTypeRelay)
            eventJson = GetRelayEventFromUi(selectedEventId, providerIndex, receiverIndex);
    }
    if (!eventJson.is_null())
    {
        auto result = RequestHelper::DoPutRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
        if (result != 200)
            LOG_ERROR << "Error while updating Event " << eventJson.dump() << "." << std::endl;
    }
    Refresh();*/
}

void EventsWidget::OnTableSelectionChanged()
{
    /*auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    FillUiWithEvent(eventJson);*/
}

void EventsWidget::OnEventTypeChanged()
{
    _eventEditorsStack->setCurrentIndex(_eventType->currentIndex());
}
