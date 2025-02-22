#pragma once

#include "PeriodSettings.hpp"
#include "Simulator.hpp"
#include "UniversalDeviceCurrentValues.hpp"

class UniversalDeviceSimulator final : public Simulator {
public:
    UniversalDeviceSimulator();

    UniversalDeviceCurrentValues GetValues() const;

    int GetPeriod() const;

    void SendValues();

protected:
    virtual void OnMessage(const ix::WebSocketMessagePtr& message) override;

private:
    PeriodSettings periodSettings;
    UniversalDeviceCurrentValues values;
};
