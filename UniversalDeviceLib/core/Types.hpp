#pragma once

#include <variant>

#include "Enums.hpp"

struct ClientActor {};

constexpr bool operator<(const ClientActor&, const ClientActor&) noexcept { return false; }

using ActorType = std::variant<ClientActor, DeviceType, EventType>;
