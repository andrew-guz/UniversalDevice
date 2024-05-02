#pragma once

#include <limits>

#include "Event.hpp"

struct ThermometerEvent final : public Event {
    float _temperature = std::numeric_limits<float>::min();
    bool _lower = true;

    ThermometerEvent() : Event(Constants::EventTypeThermometer) {}

    virtual ~ThermometerEvent() = default;
};
