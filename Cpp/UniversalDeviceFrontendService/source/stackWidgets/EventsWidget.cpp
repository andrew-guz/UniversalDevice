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
#include "ThermometerLedBrightness.h"
#include "RelayCurrentState.h"

using namespace Wt;

template<typename T>
void Hide()
{

}

template<typename T>
void Hide(T t)
{
    t->hide();
}

template<typename T, typename... Ts>
void Hide(T t, Ts... args)
{
    Hide(t);
    Hide(args...);    
}

template<typename T>
void Show()
{

}

template<typename T>
void Show(T t)
{
    t->show();
}

template<typename T, typename... Ts>
void Show(T t, Ts... args)
{
    Show(t);
    Show(args...);    
}

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
        UpdateEnableState();
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
        OnSelectionChanged();
        UpdateEnableState();
    });

    auto editorCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 1);
    auto editLayout = editorCanvas->setLayout(std::make_unique<WGridLayout>());

    _addButton = editLayout->addWidget(std::make_unique<WPushButton>("Добавить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(_addButton);
    _addButton->clicked().connect([&]{
        AddEvent();
        UpdateEnableState();
    });

    _updateButton = editLayout->addWidget(std::make_unique<WPushButton>("Применить"), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(_updateButton);
    _updateButton->clicked().connect([&]{
        UpdateEvent();
        UpdateEnableState();
    });

    auto eventGroup = editLayout->addWidget(std::make_unique<WGroupBox>("Событие"), 1, 0, 1, 2);
    auto eventLayout = eventGroup->setLayout(std::make_unique<WGridLayout>());

    eventLayout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    _name = eventLayout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    _name->keyWentUp().connect([&]() {
        UpdateEnableState();
    });
    _active = eventLayout->addWidget(std::make_unique<WCheckBox>("Активно"), 1, 0, 1, 2);
    _active->checked().connect([&]() {
        UpdateEnableState();
    });
    eventLayout->addWidget(std::make_unique<WText>("Генератор события:"), 2, 0, 1, 2);
    _providers = eventLayout->addWidget(std::make_unique<WComboBox>(), 3, 0, 1, 2);
    _providers->activated().connect([&](int index) {
        OnProviderIndexChanged(index);
        UpdateEnableState();
    });
    _providerHourText = eventLayout->addWidget(std::make_unique<WText>("Час:"), 4, 0);
    _providerHour = eventLayout->addWidget(std::make_unique<WSpinBox>(), 4, 1);
    _providerHour->setMinimum(0);
    _providerHour->setMaximum(23);
    _providerHour->keyWentUp().connect([&]() {
        UpdateEnableState();
    });
    _providerMinuteText = eventLayout->addWidget(std::make_unique<WText>("Минута:"), 5, 0);
    _providerMinute = eventLayout->addWidget(std::make_unique<WSpinBox>(), 5, 1);
    _providerMinute->setMinimum(0);
    _providerMinute->setMaximum(59);
    _providerMinute->keyWentUp().connect([&]() {
        UpdateEnableState();
    });
    _providerTemperatureText = eventLayout->addWidget(std::make_unique<WText>("Температура:"), 6, 0);
    _providerTemperature = eventLayout->addWidget(std::make_unique<WSpinBox>(), 6, 1);
    _providerTemperature->setMinimum(-40);
    _providerTemperature->setMaximum(40);
    _providerTemperature->keyWentUp().connect([&]() {
        UpdateEnableState();
    });
    _providerTemperatureLower = eventLayout->addWidget(std::make_unique<WCheckBox>("Ниже заданной"), 7, 0, 1, 2);
    _providerTemperatureLower->checked().connect([&]() {
        UpdateEnableState();
    });
    _providerRelay = eventLayout->addWidget(std::make_unique<WCheckBox>("Включено"), 8, 0, 1, 2);
    _providerRelay->checked().connect([&]() {
        UpdateEnableState();
    });
    eventLayout->addWidget(std::make_unique<WText>("Получатель события:"), 9, 0, 1, 2);
    _receivers = eventLayout->addWidget(std::make_unique<WComboBox>(), 10, 0, 1, 2);
    _receivers->activated().connect([&](int index) {
        OnReceiverIndexChanged(index);
        UpdateEnableState();
    });
    _receiverBrightnessText = eventLayout->addWidget(std::make_unique<WText>("Яркость:"), 11, 0);
    _receiverBrightness = eventLayout->addWidget(std::make_unique<WSpinBox>(), 11, 1);
    _receiverBrightness->setMinimum(MIN_BRIGHTNESS);
    _receiverBrightness->setMaximum(MAX_BRIGHTNESS);
    _receiverBrightness->keyWentUp().connect([&]() {
        UpdateEnableState();
    });
    _receiverRelay = eventLayout->addWidget(std::make_unique<WCheckBox>("Включить"), 12, 0, 1, 2);
    _receiverRelay->checked().connect([&]() {
        UpdateEnableState();
    });

    _eventsTable->setMinimumSize(200, 500);

    Refresh();
}

void EventsWidget::Initialize(const std::string& data)
{
    Refresh();
}

void EventsWidget::Clear()
{
    _eventsTableModel->UpdateData({});
    _name->setText(std::string());
    _active->setChecked(false);
    _providers->clear();
    _providerHour->setValue(0);
    _providerMinute->setValue(0);
    _providerTemperature->setValue(0);
    _providerTemperatureLower->setChecked(false);
    _providerRelay->setChecked(false);
    _receivers->clear();
    _receiverBrightness->setValue(0);
    _receiverRelay->setChecked(false);
}

void EventsWidget::Refresh()
{
    Clear();

    auto eventJsons = GetEvents();
    _eventsTableModel = std::make_shared<EventsTableModel>(eventJsons);
    _eventsTable->setModel(_eventsTableModel);
    _devices = GetDevices();
    _providers->addItem("Таймер");
    for (auto& device : _devices)
        _providers->addItem(device._name);
    OnProviderIndexChanged(0);
    for (auto& device : _devices)
        _receivers->addItem(device._name);
    OnReceiverIndexChanged(_devices.size() ? 0 : -1);
    UpdateEnableState();
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

void EventsWidget::GetEventFromUi(Event& event, const Uuid& id, int providerIndex, int receiverIndex)
{
    event._id = id;
    event._name = _name->text().toUTF8();
    event._active = _active->isChecked();
    if (providerIndex == 0)
    {
        event._type = Constants::EventTypeTimer;
        event._provider._id = Constants::PredefinedIdTimer;
        event._provider._type = Constants::DeviceTypeTimer;
    }
    else
    {
        auto providerDevice = _devices[providerIndex - 1];
        if (providerDevice._type == Constants::DeviceTypeThermometer)
            event._type = Constants::EventTypeThermometer;
        if (providerDevice._type == Constants::DeviceTypeRelay)
            event._type = Constants::EventTypeRelay;
        event._provider._id = providerDevice._id;
        event._provider._type = providerDevice._type;
    }
    auto receiverDevice = _devices[receiverIndex];
    event._receiver._id = receiverDevice._id;
    event._receiver._type = receiverDevice._type;
    if (receiverDevice._type == Constants::DeviceTypeThermometer)
    {
        ThermometerLedBrightness thermometerLedBrightness;
        thermometerLedBrightness._brightness = _receiverBrightness->value();
        event._command = thermometerLedBrightness.ToJson();
    }   
    if (receiverDevice._type == Constants::DeviceTypeRelay)
    {
        RelayCurrentState relayCurrentState;
        relayCurrentState._state = _receiverRelay->isChecked();
        event._command = relayCurrentState.ToJson();
    }
}

nlohmann::json EventsWidget::GetTimerEventFromUi(const Uuid& id, int providerIndex, int receiverIndex)
{
    TimerEvent timerEvent;
    timerEvent._id = Uuid();
    GetEventFromUi(timerEvent, id, providerIndex, receiverIndex);
    timerEvent._hour = _providerHour->value();
    timerEvent._minute = _providerMinute->value();
    return timerEvent.ToJson();
}

nlohmann::json EventsWidget::GetThermometerEventFromUi(const Uuid& id, int providerIndex, int receiverIndex)
{
    ThermometerEvent thermometerEvent;
    thermometerEvent._id = Uuid();
    GetEventFromUi(thermometerEvent, id, providerIndex, receiverIndex);
    thermometerEvent._temperature = _providerTemperature->value();
    thermometerEvent._lower = _providerTemperatureLower->isChecked();
    return thermometerEvent.ToJson();
}

nlohmann::json EventsWidget::GetRelayEventFromUi(const Uuid& id, int providerIndex, int receiverIndex)
{
    RelayEvent relayEvent;
    relayEvent._id = Uuid();
    GetEventFromUi(relayEvent, id, providerIndex, receiverIndex);
    relayEvent._state = _providerRelay->isChecked();
    return relayEvent.ToJson();
}

void EventsWidget::FillUiWithEvent(const nlohmann::json& eventJson)
{
    auto event = JsonExtension::CreateFromJson<Event>(eventJson);
    _name->setText(event._name);
    _active->setChecked(event._active);
    if (event._provider._id == Constants::PredefinedIdTimer)
    {
        _providers->setCurrentIndex(0);
        OnProviderIndexChanged(0);
        auto timerEvent = JsonExtension::CreateFromJson<TimerEvent>(eventJson);
        _providerHour->setValue(timerEvent._hour);
        _providerMinute->setValue(timerEvent._minute);
    }
    else
    {
        auto providerDeviceIter = std::find_if(_devices.begin(), _devices.end(), [&](const auto& device){ return device._id == event._provider._id; });
        _providers->setCurrentIndex(providerDeviceIter - _devices.begin() + 1);
        OnProviderIndexChanged(providerDeviceIter - _devices.begin() + 1);
        auto providerDevice = *providerDeviceIter;
        if (providerDevice._type == Constants::DeviceTypeThermometer)
        {
            auto thermometerEvent = JsonExtension::CreateFromJson<ThermometerEvent>(eventJson);
            _providerTemperature->setValue(thermometerEvent._temperature);
            _providerTemperatureLower->setChecked(thermometerEvent._lower);
        }
        if (providerDevice._type == Constants::DeviceTypeRelay)
        {
            auto relayEvent = JsonExtension::CreateFromJson<RelayEvent>(eventJson);
            _providerRelay->setChecked(relayEvent._state);
        }
    }
    auto receiverDeviceIter = std::find_if(_devices.begin(), _devices.end(), [&](const auto& device){ return device._id == event._receiver._id; });
    _receivers->setCurrentIndex(receiverDeviceIter - _devices.begin());
    OnReceiverIndexChanged(receiverDeviceIter - _devices.begin());
    auto receiverDevice = *receiverDeviceIter;
    if (receiverDevice._type == Constants::DeviceTypeThermometer)
    {
        auto thermometerLedBrightness = JsonExtension::CreateFromJson<ThermometerLedBrightness>(event._command);
        _receiverBrightness->setValue(thermometerLedBrightness._brightness);
    }
    if (receiverDevice._type == Constants::DeviceTypeRelay)
    {
        auto relayCurrentState = JsonExtension::CreateFromJson<RelayCurrentState>(event._command);
        _receiverRelay->setChecked(relayCurrentState._state);
    }
}

void EventsWidget::AddEvent()
{
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
    Refresh();
}

void EventsWidget::DeleteEvent()
{
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    auto result = RequestHelper::DoDeleteRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService, eventJson);
    if (result != 200)
            LOG_ERROR << "Error while deleting Event " << eventJson.dump() << "." << std::endl;
    Refresh();
}

void EventsWidget::UpdateEvent()
{
    auto selectedIndexes = _eventsTable->selectedIndexes();
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
    Refresh();    
}

void EventsWidget::OnSelectionChanged()
{
    auto selectedIndexes = _eventsTable->selectedIndexes();
    if (selectedIndexes.empty())
        return;
    auto eventJson = cpp17::any_cast<nlohmann::json>(_eventsTable->model()->data(*selectedIndexes.begin(), ItemDataRole::User));
    FillUiWithEvent(eventJson);
}

void EventsWidget::OnProviderIndexChanged(int index)
{
    if (index == -1) //nothing
        Hide(_providerHourText, _providerHour, _providerMinuteText, _providerMinute, _providerTemperatureText, _providerTemperature, _providerTemperatureLower, _providerRelay);
    else if (index == 0) //timer
    {
        Show(_providerHourText, _providerHour, _providerMinuteText, _providerMinute);
        Hide(_providerTemperatureText, _providerTemperature, _providerTemperatureLower, _providerRelay);
    }
    else
    {
        if (_devices.empty() ||
            (size_t)(index - 1) >= _devices.size())
            Hide(_providerHourText, _providerHour, _providerMinuteText, _providerMinute, _providerTemperatureText, _providerTemperature, _providerTemperatureLower, _providerRelay);
        else
        {
            auto device = _devices[index - 1];
            if (device._type == Constants::DeviceTypeThermometer)
            {
                Show(_providerTemperatureText, _providerTemperature, _providerTemperatureLower);
                Hide(_providerHourText, _providerHour, _providerMinuteText, _providerMinute, _providerRelay);
            }
            else if (device._type == Constants::DeviceTypeRelay)
            {
                Show(_providerRelay);
                Hide(_providerHourText, _providerHour, _providerMinuteText, _providerMinute, _providerTemperatureText, _providerTemperature, _providerTemperatureLower);
            }    
        }
    }
}

void EventsWidget::OnReceiverIndexChanged(int index)
{
    if (index == -1) //nothing
        Hide(_receiverBrightnessText, _receiverBrightness, _receiverRelay);
    else if (_devices.empty() ||
        (size_t)index >= _devices.size())
        Hide(_receiverBrightnessText, _receiverBrightness, _receiverRelay);
    else
    {
        auto device = _devices[index];
        if (device._type == Constants::DeviceTypeThermometer)
        {
            Show(_receiverBrightnessText, _receiverBrightness);
            Hide(_receiverRelay);
        }
        else if (device._type == Constants::DeviceTypeRelay)
        {
            Show(_receiverRelay);
            Hide(_receiverBrightnessText, _receiverBrightness);
        }    
    }
}

void EventsWidget::UpdateEnableState()
{
    _deleteButton->setEnabled(_eventsTable->selectedIndexes().size());
    auto addUpdate = _name->text().toUTF8().size() &&
        _providers->currentIndex() >= 0 &&
        (size_t)_providers->currentIndex() < _devices.size() + 1 &&
        ((_providerHour->isVisible() && _providerHour->validate() == ValidationState::Valid) || _providerHour->isHidden()) &&
        ((_providerMinute->isVisible() && _providerMinute->validate() == ValidationState::Valid) || _providerMinute->isHidden()) &&
        ((_providerTemperature->isVisible() && _providerTemperature->validate() == ValidationState::Valid) || _providerTemperature->isHidden()) &&
        _receivers->currentIndex() >= 0 &&
        (size_t)_receivers->currentIndex() < _devices.size() &&
        ((_receiverBrightness->isVisible() && _receiverBrightness->validate() == ValidationState::Valid) || _receiverBrightness->isHidden());
    _addButton->setEnabled(addUpdate);
    _updateButton->setEnabled(addUpdate);
}
