#pragma once

#include <string>

#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>

#include "Device.hpp"
#include "Enums.hpp"
#include "Uuid.hpp"

class DeviceButton final : public Wt::WPushButton {
public:
    DeviceButton(int port, const Device& device);

    virtual ~DeviceButton();

private:
    void Refresh();

private:
    int _port;
    DeviceType _deviceType;
    Uuid _deviceId;
    std::string _text;
    Wt::WTimer* _refreshTimer = nullptr;
};
