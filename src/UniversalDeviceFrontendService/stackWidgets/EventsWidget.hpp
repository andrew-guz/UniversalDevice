#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WTableView.h>
#include <Wt/WVBoxLayout.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "ApplicationSettings.hpp"
#include "BaseEventEditor.hpp"
#include "BaseStackWidget.hpp"
#include "Device.hpp"
#include "EventsTableModel.hpp"
#include "IStackHolder.hpp"
#include "Uuid.hpp"

class EventsWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    EventsWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

    virtual ~EventsWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    std::vector<nlohmann::json> GetEvents();

    Devices GetDevices();

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
    Devices _devices;
    Wt::WVBoxLayout* _mainLayout;
    std::shared_ptr<EventsTableModel> _eventsTableModel;
    Wt::WTableView* _eventsTable;
    Wt::WComboBox* _eventType;
    Wt::WVBoxLayout* _eventEditorLayout;
    BaseEventEditor* _eventEditor;
};
