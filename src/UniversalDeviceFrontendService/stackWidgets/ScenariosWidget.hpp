#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGlobal.h>
#include <Wt/WGridLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WVBoxLayout.h>

#include "ApplicationSettings.hpp"
#include "BaseStackWidget.hpp"
#include "Command.hpp"
#include "Device.hpp"
#include "Event.hpp"
#include "EventReceiverWidget.hpp"
#include "IStackHolder.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"

class ScenariosWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    ScenariosWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

    virtual ~ScenariosWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    void AddScenario();

    void OnSelection(int index);

    void UpdateScenario();

    void DeleteScenario();

    std::set<Uuid> GetSelectedEventIndexes(const std::vector<Wt::WCheckBox*>& container) const;

    void SetSelectedEventIndexes(const std::vector<Wt::WCheckBox*>& container, const std::set<std::size_t>& indexes);

    void CleanupCommandsCanvas();

    Devices GetDevices();

    void FillCommandsCanvas(const std::unordered_map<Uuid, Command>& commands);

    bool IsUiValid() const;

private:
    Wt::WVBoxLayout* _mainLayout;
    Wt::WSelectionBox* _scenariosList;
    Wt::WLineEdit* _nameEditor;
    Wt::WGroupBox* _activateEventsGroup;
    Wt::WGroupBox* _deactivateEventsGroup;
    WContainerWidget* _commandsCanvas;
    Wt::WGridLayout* _commandsLayout;
    EventReceiverWidget* _commandEditor;
    Wt::WPushButton* _addCommandButton;
    std::vector<Wt::WWidget*> _commandsWidgets;
    std::vector<Wt::WCheckBox*> _activatedEvents;
    std::vector<Wt::WCheckBox*> _deactivatedEvents;
    std::vector<Scenario> _scenarios;
    std::vector<Event> _events;
};
