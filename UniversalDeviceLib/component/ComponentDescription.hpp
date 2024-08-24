#pragma once

#include "Types.hpp"
#include "Uuid.hpp"

struct ComponentDescription {
    ActorType _type;
    Uuid _id;

    virtual ~ComponentDescription() = default;

    bool isClientType() const;

    bool isDeviceType() const;

    bool isEventType() const;

    DeviceType getDeviceType() const;

    EventType getEventType() const;
};
