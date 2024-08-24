#include "DeviceComboBox.hpp"

#include "Defines.hpp"
#include "RequestHelper.hpp"

using namespace std;

DeviceComboBox::DeviceComboBox() : WComboBox() {}

void DeviceComboBox::SetDevices(const std::vector<ExtendedComponentDescription>& devices) {
    clear();
    _devices = devices;
    for (auto& device : _devices)
        addItem(device.GetDisplayStringWithGroup());
}

bool DeviceComboBox::IsValid() const {
    size_t index = currentIndex();
    return index >= 0 && index < _devices.size();
}

ComponentDescription DeviceComboBox::GetSelectedDevice() const {
    if (!IsValid())
        return {};
    return _devices[currentIndex()];
}

void DeviceComboBox::SetSelectedDevice(const Uuid& deviceId) {
    auto iter = std::find_if(_devices.begin(), _devices.end(), [&deviceId](const auto& device) { return device._id == deviceId; });
    if (iter == _devices.end())
        setCurrentIndex(-1);
    else
        setCurrentIndex(iter - _devices.begin());
}
