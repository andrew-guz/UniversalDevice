#pragma once

#include "PeriodSettings.hpp"
#include "Simulator.hpp"

class RelaySimulator final : public Simulator {
public:
    RelaySimulator();

    bool GetState() const;

    int GetPeriod() const;

    void SendState();

protected:
    virtual void OnMessage(const ix::WebSocketMessagePtr& message) override;

private:
    PeriodSettings periodSettings;
    bool state = false;
};
