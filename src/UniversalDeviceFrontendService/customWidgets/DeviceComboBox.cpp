#include "DeviceComboBox.hpp"

#include <algorithm>
#include <cstddef>

#include <Wt/WComboBox.h>

#include "Device.hpp"
#include "DeviceDescription.hpp"
#include "Uuid.hpp"

using namespace std;

DeviceComboBox::DeviceComboBox() :
    WComboBox() {}

void DeviceComboBox::SetDevices(const Devices& devices) {
    clear();
    _devices = devices;
    for (auto& device : _devices)
        addItem(device.GetDisplayStringWithGroup());
}

bool DeviceComboBox::IsValid() const {
    size_t index = currentIndex();
    return index >= 0 && index < _devices.size();
}

DeviceDescription DeviceComboBox::GetSelectedDevice() const {
    if (!IsValid())
        return {};
    return DeviceDescription{
        ._type = _devices[currentIndex()]._type,
        ._id = _devices[currentIndex()]._id,
    };
}

void DeviceComboBox::SetSelectedDevice(const Uuid& deviceId) {
    auto iter = std::find_if(_devices.begin(), _devices.end(), [&deviceId](const auto& device) { return device._id == deviceId; });
    if (iter == _devices.end())
        setCurrentIndex(-1);
    else
        setCurrentIndex(iter - _devices.begin());
}
