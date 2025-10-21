#include "DatabaseMarshaling.hpp"

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "DbExtension.hpp"
#include "Device.hpp"
#include "Enums.hpp"
#include "ExtendedComponentDescription.hpp"
#include "ExtendedMotionRelayCurrentState.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "ExtendedThermometerCurrentValue.hpp"
#include "ExtendedUniversalDeviceCurrentValues.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayValue.hpp"
#include "ThermometerValue.hpp"
#include "Types.hpp"
#include "Uuid.hpp"

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, Device& object) {
    if (dbStrings.size() % 2 == 0) {
        auto id = DbExtension::FindValueByName<Uuid>(dbStrings, "id");
        auto type = DbExtension::FindValueByName<DeviceType>(dbStrings, "type");
        auto name = DbExtension::FindValueByName<std::string>(dbStrings, "name");
        auto group = DbExtension::FindValueByName<std::string>(dbStrings, "grp");
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        if (type.has_value() && id.has_value() && timestamp.has_value()) {
            object._id = id.value();
            object._type = type.value();
            object._name = name.value_or(std::string{});
            object._group = group.value_or(std::string{});
            object._timestamp = timestamp.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedComponentDescription& object) {
    if (dbStrings.size() % 2 == 0) {
        auto id = DbExtension::FindValueByName<Uuid>(dbStrings, "id");
        auto type = DbExtension::FindValueByName<ActorType>(dbStrings, "type");
        auto name = DbExtension::FindValueByName<std::string>(dbStrings, "name");
        auto group = DbExtension::FindValueByName<std::string>(dbStrings, "grp");
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        if (type.has_value() && id.has_value() && timestamp.has_value()) {
            object._id = id.value();
            object._type = type.value();
            object._name = name.value_or(std::string{});
            object._group = group.value_or(std::string{});
            object._timestamp = timestamp.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedMotionRelayCurrentState& object) {
    if (dbStrings.size() % 2 == 0) {
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        auto motion = DbExtension::FindValueByName<bool>(dbStrings, "motion");
        auto state = DbExtension::FindValueByName<int>(dbStrings, "state");
        if (timestamp.has_value() && motion.has_value() && state.has_value()) {
            object._timestamp = timestamp.value();
            object._motion = motion.value();
            object._state = state.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedRelayCurrentState& object) {
    if (dbStrings.size() % 2 == 0) {
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        auto state = DbExtension::FindValueByName<int>(dbStrings, "state");
        if (timestamp.has_value() && state.has_value()) {
            object._timestamp = timestamp.value();
            object._state = state.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedThermometerCurrentValue& object) {
    if (dbStrings.size() % 2 == 0) {
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        auto value = DbExtension::FindValueByName<float>(dbStrings, "value");
        if (timestamp.has_value() && value.has_value()) {
            object._timestamp = timestamp.value();
            object._value = value.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedUniversalDeviceCurrentValues& object) {
    if (dbStrings.size() % 2 == 0) {
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        auto values = DbExtension::FindValueByName<std::string>(dbStrings, "values");
        if (timestamp.has_value() && values.has_value()) {
            object._timestamp = timestamp.value();
            object._values = nlohmann::json::parse(values.value()).get<UniversalDeviceCurrentValues>()._values;
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, RelayValue& object) {
    if (dbStrings.size() % 2 == 0) {
        auto state = DbExtension::FindValueByName<int>(dbStrings, "state");
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        if (state.has_value() && timestamp.has_value()) {
            object._state = state.value();
            object._timestamp = timestamp.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ThermometerValue& object) {
    if (dbStrings.size() % 2 == 0) {
        auto value = DbExtension::FindValueByName<float>(dbStrings, "value");
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        if (value.has_value() && timestamp.has_value()) {
            object._value = value.value();
            object._timestamp = timestamp.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}
