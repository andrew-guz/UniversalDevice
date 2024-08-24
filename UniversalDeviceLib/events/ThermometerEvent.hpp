#pragma once

#include <limits>

#include "Enums.hpp"
#include "Event.hpp"

struct ThermometerEvent final : public Event {
    float _temperature = std::numeric_limits<float>::min();
    bool _lower = true;

    ThermometerEvent() : Event(EventType::Thermometer) {}

    virtual ~ThermometerEvent() = default;
};
