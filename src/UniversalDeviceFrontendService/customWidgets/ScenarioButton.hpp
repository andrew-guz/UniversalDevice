#pragma once

#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>

#include "Scenario.hpp"

class ScenarioButton final : public Wt::WPushButton {
public:
    ScenarioButton(int port, Scenario scenario);

    virtual ~ScenarioButton();

private:
    void Refresh();

private:
    int _port;
    Scenario _scenario;
    Wt::WTimer* _refreshTimer = nullptr;
};
