#ifndef _DEVICE_BUTTON_H_
#define _DEVICE_BUTTON_H_

#include <string>

#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>

#include "ExtendedComponentDescription.h"
#include "Uuid.h"

class DeviceButton final : public Wt::WPushButton {
public:
    DeviceButton(int port, const ExtendedComponentDescription& description);

    virtual ~DeviceButton();

private:
    void Refresh();

private:
    int _port;
    std::string _deviceType;
    Uuid _deviceId;
    std::string _text;
    Wt::WTimer* _refreshTimer = nullptr;
};

#endif // _DEVICE_BUTTON_H_
