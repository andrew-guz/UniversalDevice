#include "ComponentDescription.hpp"

bool ComponentDescription::isClientType() const { return std::holds_alternative<ClientActor>(this->_type); }

bool ComponentDescription::isDeviceType() const { return std::holds_alternative<DeviceType>(this->_type); }

bool ComponentDescription::isEventType() const { return std::holds_alternative<EventType>(this->_type); }

DeviceType ComponentDescription::getDeviceType() const { return std::get<DeviceType>(this->_type); }

EventType ComponentDescription::getEventType() const { return std::get<EventType>(this->_type); }
