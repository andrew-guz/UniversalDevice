#ifndef _DEVICE_COMBO_BOX_H_
#define _DEVICE_COMBO_BOX_H_

#include <Wt/WComboBox.h>

#include "ExtendedComponentDescription.h"

class DeviceComboBox final : public Wt::WComboBox
{
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

#endif //_DEVICE_COMBO_BOX_H_
