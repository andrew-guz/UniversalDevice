#include "DeviceComboBox.h"

#include "Defines.h"
#include "RequestHelper.h"

using namespace std;

DeviceComboBox::DeviceComboBox(const std::vector<ComponentDescription>& devices) :
    WComboBox(),
    _devices(devices)
{

}

bool DeviceComboBox::IsValid() const
{
    size_t index = currentIndex();
    return index >= 0 &&
        index < _devices.size();
}

ComponentDescription DeviceComboBox::GetSelectedDevice() const
{
    if (!IsValid())
        return {};
    return _devices[currentIndex()];
}

void DeviceComboBox::SetSelectedDevice(const Uuid& deviceId)
{
    auto iter = std::find_if(_devices.begin(), _devices.end(), [&deviceId](const auto& device){ return device._id == deviceId; });
    if (iter == _devices.end())
        setCurrentIndex(-1);
    else
        setCurrentIndex(iter - _devices.begin());
}
