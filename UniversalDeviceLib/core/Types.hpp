#pragma once

#include "Enums.hpp"

#include <variant>

struct ClientActor {};

constexpr bool operator<(const ClientActor&, const ClientActor& ) noexcept { return false; }

using ActorType = std::variant<ClientActor, DeviceType, EventType>;
