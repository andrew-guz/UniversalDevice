#pragma once

#include <set>
#include <vector>

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WVBoxLayout.h>

#include "BaseStackWidget.hpp"
#include "Event.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"

class ScenariosWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    ScenariosWidget(IStackHolder* stackHolder, const Settings& settings);

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

    bool IsUiValid() const;

private:
    Wt::WVBoxLayout* _mainLayout;
    Wt::WSelectionBox* _scenariosList;
    Wt::WLineEdit* _nameEditor;
    Wt::WGroupBox* _activateEventsGroup;
    Wt::WGroupBox* _deactivateEventsGroup;
    std::vector<Wt::WCheckBox*> _activatedEvents;
    std::vector<Wt::WCheckBox*> _deactivatedEvents;
    std::vector<Scenario> _scenarios;
    std::vector<Event> _events;
};
