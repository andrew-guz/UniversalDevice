#include "Provider.hpp"

#include <variant>

#include <boost/hof/match.hpp>

#include "Constants.hpp"
#include "DeviceDescription.hpp"
#include "Uuid.hpp"

Uuid GetProviderId(const Provider& provider) {
    return std::visit(boost::hof::match([](const DeviceDescription& device) { return device._id; },
                                        [](const ClientProvider& client) { return Constants::PredefinedIdClient; },
                                        [](const EventProvider& event) { return event._id; }),
                      provider);
}
