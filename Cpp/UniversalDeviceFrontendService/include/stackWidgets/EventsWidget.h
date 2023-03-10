#ifndef _EVENTS_WIDGET_H_
#define _EVENTS_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WTableView.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WCheckBox.h>
#include <nlohmann/json.hpp>

#include "BaseStackWidget.h"
#include "Settings.h"
#include "EventsTableModel.h"
#include "ExtendedComponentDescription.h"
#include "Event.h"

class EventsWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    EventsWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~EventsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    std::vector<nlohmann::json> GetEvents();

    std::vector<ExtendedComponentDescription> GetDevices();    
    
    void GetEventFromUi(Event& event, const Uuid& id, int providerIndex, int receiverIndex);

    nlohmann::json GetTimerEventFromUi(const Uuid& id, int providerIndex, int receiverIndex);

    nlohmann::json GetThermometerEventFromUi(const Uuid& id, int providerIndex, int receiverIndex);

    nlohmann::json GetRelayEventFromUi(const Uuid& id, int providerIndex, int receiverIndex);

    void FillUiWithEvent(const nlohmann::json& eventJson);

    void AddEvent();

    void DeleteEvent();

    void UpdateEvent();

    void OnSelectionChanged();

    void OnProviderIndexChanged(int index);

    void OnReceiverIndexChanged(int index);

    void UpdateEnableState();

private:
    Wt::WGridLayout*                            _mainLayout;
    Wt::WPushButton*                            _deleteButton;
    Wt::WPushButton*                            _addButton;
    Wt::WPushButton*                            _updateButton;
    std::shared_ptr<EventsTableModel>           _eventsTableModel;
    Wt::WTableView*                             _eventsTable;
    std::vector<ExtendedComponentDescription>   _devices;
    Wt::WLineEdit*                              _name;
    Wt::WCheckBox*                              _active;
    Wt::WComboBox*                              _providers;
    Wt::WText*                                  _providerHourText;
    Wt::WSpinBox*                               _providerHour;
    Wt::WText*                                  _providerMinuteText;
    Wt::WSpinBox*                               _providerMinute;
    Wt::WText*                                  _providerTemperatureText;
    Wt::WSpinBox*                               _providerTemperature;
    Wt::WCheckBox*                              _providerTemperatureLower;
    Wt::WCheckBox*                              _providerRelay;
    Wt::WComboBox*                              _receivers;
    Wt::WText*                                  _receiverBrightnessText;
    Wt::WSpinBox*                               _receiverBrightness;
    Wt::WCheckBox*                              _receiverRelay;
};

#endif //_EVENTS_WIDGET_H_
