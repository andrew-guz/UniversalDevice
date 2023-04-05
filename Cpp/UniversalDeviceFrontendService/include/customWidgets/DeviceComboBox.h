#ifndef _DEVICE_COMBO_BOX_H_
#define _DEVICE_COMBO_BOX_H_

#include <Wt/WComboBox.h>

#include "ComponentDescription.h"

class DeviceComboBox : public Wt::WComboBox
{
public:
    DeviceComboBox(const std::vector<ComponentDescription>& devices);

    virtual ~DeviceComboBox() = default;

    bool IsDeviceSelected() const;

    ComponentDescription GetSelectedDevice() const;

    void SetSelectedDevice(const Uuid& deviceId);

private:
    std::vector<ComponentDescription> _devices;
};

#endif //_DEVICE_COMBO_BOX_H_
