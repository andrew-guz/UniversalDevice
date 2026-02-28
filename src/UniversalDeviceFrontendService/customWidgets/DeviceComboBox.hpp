#pragma once

#include <Wt/WComboBox.h>

#include "Device.hpp"
#include "DeviceDescription.hpp"
#include "Uuid.hpp"

class DeviceComboBox final : public Wt::WComboBox {
public:
    DeviceComboBox();

    virtual ~DeviceComboBox() = default;

    void SetDevices(const Devices& devices);

    bool IsValid() const;

    DeviceDescription GetSelectedDevice() const;

    void SetSelectedDevice(const Uuid& deviceId);

private:
    Devices _devices;
};
