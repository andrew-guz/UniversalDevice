#pragma once

#include <Wt/WComboBox.h>

#include "ExtendedComponentDescription.hpp"

class DeviceComboBox final : public Wt::WComboBox {
public:
    DeviceComboBox();

    virtual ~DeviceComboBox() = default;

    void SetDevices(const std::vector<ExtendedComponentDescription>& devices);

    bool IsValid() const;

    ComponentDescription GetSelectedDevice() const;

    void SetSelectedDevice(const Uuid& deviceId);

private:
    std::vector<ExtendedComponentDescription> _devices;
};
