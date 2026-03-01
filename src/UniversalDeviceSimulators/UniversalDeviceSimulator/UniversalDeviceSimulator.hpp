#pragma once

#include <ixwebsocket/IXWebSocketMessage.h>

#include "PeriodSettings.hpp"
#include "Simulator.hpp"
#include "UniversalValue.hpp"

class UniversalDeviceSimulator final : public Simulator {
public:
    UniversalDeviceSimulator();

    UniversalValue GetValues() const;

    int GetPeriod() const;

    void SendValues();

protected:
    virtual void OnMessage(const ix::WebSocketMessagePtr& message) override;

private:
    PeriodSettings periodSettings;
    UniversalValue values;
};
