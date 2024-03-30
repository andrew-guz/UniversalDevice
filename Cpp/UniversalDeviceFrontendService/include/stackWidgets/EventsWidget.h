#ifndef _EVENTS_WIDGET_H_
#define _EVENTS_WIDGET_H_

#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WTableView.h>

#include <nlohmann/json.hpp>

#include "BaseEventEditor.h"
#include "BaseStackWidget.h"
#include "Event.h"
#include "EventsTableModel.h"
#include "ExtendedComponentDescription.h"
#include "Settings.h"

class EventsWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    EventsWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~EventsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    std::vector<nlohmann::json> GetEvents();

    std::vector<ExtendedComponentDescription> GetDevices();

    void AddEvent();

    void DeleteEvent();

    void UpdateEvent();

    void OnTableSelectionChanged();

    void OnEventTypeChanged();

    BaseEventEditor* GetCurrentEventEditor() const;

    Event* CreateNewEventFromEditor(BaseEventEditor* eventEditor) const;

    Event* CreateNewEventFromJson(const nlohmann::json& eventJson) const;

    nlohmann::json GetSelectedEventJsonFromTable() const;

    Uuid GetSelectedEventIdFromTable() const;

    void ShowIncorrectEventMsgBox();

private:
    std::vector<ExtendedComponentDescription> _devices;
    Wt::WGridLayout* _mainLayout;
    Wt::WPushButton* _deleteButton;
    Wt::WPushButton* _addButton;
    Wt::WPushButton* _updateButton;
    std::shared_ptr<EventsTableModel> _eventsTableModel;
    Wt::WTableView* _eventsTable;
    Wt::WComboBox* _eventType;
    Wt::WStackedWidget* _eventEditorsStack;
};

#endif //_EVENTS_WIDGET_H_
