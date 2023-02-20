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
#include "Event.h"
#include "TimerEvent.h"
#include "ThermometerEvent.h"
#include "RelayEvent.h"

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

    auto deleteEventButton = tableLayout->addWidget(std::make_unique<WPushButton>("Удалить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(deleteEventButton);
    deleteEventButton->clicked().connect([&]{
        DeleteEvent();
    });

    _eventsTableModel = std::make_shared<EventsTableModel>(_eventJsons);

    _eventsTable = tableLayout->addWidget(std::make_unique<WTableView>(), 1, 0);
    _eventsTable->setColumnResizeEnabled(false);
    _eventsTable->setAlternatingRowColors(true);
    _eventsTable->setSelectionMode(SelectionMode::Single);
    _eventsTable->setSelectionBehavior(SelectionBehavior::Rows);
    _eventsTable->setEditTriggers(EditTrigger::None);
    _eventsTable->setModel(_eventsTableModel);

    auto editorCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1, 1);
    auto editLayout = editorCanvas->setLayout(std::make_unique<WGridLayout>());

    auto addEventButton = editLayout->addWidget(std::make_unique<WPushButton>("Добавить"), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(addEventButton);
    addEventButton->clicked().connect([&]{
        AddEvent();
    });

    auto updateEventButton = editLayout->addWidget(std::make_unique<WPushButton>("Применить"), 0, 1, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(updateEventButton);
    updateEventButton->clicked().connect([&]{
        UpdateEvent();
    });

    auto eventGroup = editLayout->addWidget(std::make_unique<WGroupBox>("Событие"), 1, 0, 1, 2);
    auto eventLayout = eventGroup->setLayout(std::make_unique<WGridLayout>());

    _active = eventLayout->addWidget(std::make_unique<WCheckBox>("Активно"), 0, 0, 1, 2);
    eventLayout->addWidget(std::make_unique<WText>("Генератор события:"), 1, 0, 1, 2);
    _providers = eventLayout->addWidget(std::make_unique<WComboBox>(), 2, 0, 1, 2);
    _providers->activated().connect([&](int index) {
        OnProviderIndexChanged(index);
    });
    _providerHourText = eventLayout->addWidget(std::make_unique<WText>("Час:"), 3, 0);
    _providerHour = eventLayout->addWidget(std::make_unique<WSpinBox>(), 3, 1);
    _providerHour->setMinimum(0);
    _providerHour->setMaximum(23);
    _providerMinuteText = eventLayout->addWidget(std::make_unique<WText>("Минута:"), 4, 0);
    _providerMinute = eventLayout->addWidget(std::make_unique<WSpinBox>(), 4, 1);
    _providerMinute->setMinimum(0);
    _providerMinute->setMaximum(59);
    _providerTemperatureText = eventLayout->addWidget(std::make_unique<WText>("Температура:"), 5, 0);
    _providerTemperature = eventLayout->addWidget(std::make_unique<WSpinBox>(), 5, 1);
    _providerTemperature->setMinimum(-40);
    _providerTemperature->setMaximum(40);
    _providerTemperatureLower = eventLayout->addWidget(std::make_unique<WCheckBox>("Ниже заданной"), 6, 0, 1, 2);
    _providerRelay = eventLayout->addWidget(std::make_unique<WCheckBox>("Включено"), 7, 0, 1, 2);
    eventLayout->addWidget(std::make_unique<WText>("Получатель события:"), 8, 0, 1, 2);
    _receivers = eventLayout->addWidget(std::make_unique<WComboBox>(), 9, 0, 1, 2);
    _receivers->activated().connect([&](int index) {
        OnReceiverIndexChanged(index);
    });
    _receiverBrightnessText = eventLayout->addWidget(std::make_unique<WText>("Яркость:"), 10, 0);
    _receiverBrightness = eventLayout->addWidget(std::make_unique<WSpinBox>(), 10, 1);
    _receiverBrightness->setMinimum(MIN_BRIGHTNESS);
    _receiverBrightness->setMaximum(MAX_BRIGHTNESS);
    _receiverRelay = eventLayout->addWidget(std::make_unique<WCheckBox>("Включить"), 11, 0, 1, 2);

    Refresh();
}

void EventsWidget::Initialize(const std::string& data)
{
    Refresh();
}

void EventsWidget::Clear()
{
    _eventJsons.clear();
    _eventsTableModel->UpdateData(_eventJsons);
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

    _eventJsons = GetEvents();
    _eventsTableModel->UpdateData(_eventJsons);
    _devices = GetDevices();
    _providers->addItem("Таймер");
    for (auto& device : _devices)
        _providers->addItem(device._name);
    OnProviderIndexChanged(0);
    for (auto& device : _devices)
        _receivers->addItem(device._name);
    OnReceiverIndexChanged(_devices.size() ? 0 : -1);
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
    if (_devices.empty())
        return;
    auto providerIndex = _providers->currentIndex();
    if (providerIndex == 0)
    {
        //timer
    }
    else
    {

    }
}

void EventsWidget::DeleteEvent()
{
    
}

void EventsWidget::UpdateEvent()
{
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
