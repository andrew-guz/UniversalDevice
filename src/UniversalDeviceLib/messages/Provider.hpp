#pragma once

#include <variant>

#include <boost/hof/match.hpp>

#include "DeviceDescription.hpp"
#include "Uuid.hpp"

// Always has id == Constants::PredefinedIdClient
struct ClientProvider {};

struct EventProvider {
    Uuid _id;
};

using Provider = std::variant<DeviceDescription, ClientProvider, EventProvider>;

Uuid GetProviderId(const Provider& provider);
