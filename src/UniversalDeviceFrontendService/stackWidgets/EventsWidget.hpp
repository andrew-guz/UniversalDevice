#pragma once

#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WTableView.h>
#include <Wt/WVBoxLayout.h>
#include <nlohmann/json.hpp>

#include "BaseEventEditor.hpp"
#include "BaseStackWidget.hpp"
#include "EventsTableModel.hpp"
#include "ExtendedComponentDescription.hpp"
#include "Settings.hpp"

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

    nlohmann::json CreateNewEventFromEditor(BaseEventEditor* eventEditor) const;

    nlohmann::json GetSelectedEventJsonFromTable() const;

    Uuid GetSelectedEventIdFromTable() const;

    void ShowIncorrectEventMsgBox();

private:
    std::vector<ExtendedComponentDescription> _devices;
    Wt::WVBoxLayout* _mainLayout;
    std::shared_ptr<EventsTableModel> _eventsTableModel;
    Wt::WTableView* _eventsTable;
    Wt::WComboBox* _eventType;
    Wt::WVBoxLayout* _eventEditorLayout;
    BaseEventEditor* _eventEditor;
};
