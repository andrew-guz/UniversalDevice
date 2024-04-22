#pragma once

#include "Parameters.hpp"
#include "PeriodSettings.h"
#include "Simulator.hpp"

class ThermometerSimulator final : public Simulator {
public:
    ThermometerSimulator();

    float GetTemperature();

    int GetPeriod() const;

    void SendTemperature();

protected:
    virtual void OnMessage(const ix::WebSocketMessagePtr& message) override;

private:
    Parameters parameters;
    PeriodSettings periodSettings;
    float temperature;
};
