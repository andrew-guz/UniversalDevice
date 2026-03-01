#pragma once

#include <string>

#include "Enums.hpp"
#include "Event.hpp"
#include "Uuid.hpp"

Uuid GetEventId(const Event& event);

std::string GetEventName(const Event& event);

bool GetEventActivity(const Event& event);

EventType GetEventType(const Event& event);
