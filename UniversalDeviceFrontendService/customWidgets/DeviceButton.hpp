#pragma once

#include <string>

#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>

#include "ExtendedComponentDescription.hpp"
#include "Types.hpp"
#include "Uuid.hpp"

class DeviceButton final : public Wt::WPushButton {
public:
    DeviceButton(int port, const ExtendedComponentDescription& description);

    virtual ~DeviceButton();

private:
    void Refresh();

private:
    int _port;
    ActorType _deviceType;
    Uuid _deviceId;
    std::string _text;
    Wt::WTimer* _refreshTimer = nullptr;
};
