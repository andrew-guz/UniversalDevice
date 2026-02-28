#include "DatabaseMarshaling.hpp"

#include <chrono>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "DbExtension.hpp"
#include "Device.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "MotionRelayValue.hpp"
#include "RelayValue.hpp"
#include "Scenario.hpp"
#include "ThermometerValue.hpp"
#include "UniversalData.hpp"
#include "UniversalValue.hpp"
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
void FromDbStrings(const std::vector<std::string>& dbStrings, Event& object) {
    if (dbStrings.size() % 2 == 0) {
        auto event = DbExtension::FindValueByName<std::string>(dbStrings, "event");
        if (event.has_value()) {
            nlohmann::json::parse(event.value()).get_to<Event>(object);
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, Scenario& object) {
    if (dbStrings.size() % 2 == 0) {
        auto scenario = DbExtension::FindValueByName<std::string>(dbStrings, "scenario");
        if (scenario.has_value()) {
            nlohmann::json::parse(scenario.value()).get_to<Scenario>(object);
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, MotionRelayValue& object) {
    if (dbStrings.size() % 2 == 0) {
        auto motion = DbExtension::FindValueByName<bool>(dbStrings, "motion");
        auto state = DbExtension::FindValueByName<int>(dbStrings, "state");
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        if (motion.has_value() && state.has_value() && timestamp.has_value()) {
            object._motion = motion.value();
            object._state = state.value();
            object._timestamp = timestamp.value();
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

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, UniversalValue& object) {
    if (dbStrings.size() % 2 == 0) {
        auto values = DbExtension::FindValueByName<std::string>(dbStrings, "values");
        auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
        if (values.has_value() && timestamp.has_value()) {
            const nlohmann::json valuesJson = nlohmann::json::parse(values.value());
            object._values = valuesJson.get<std::map<std::string, UniversalData>>();
            object._timestamp = timestamp.value();
        }
    } else
        LOG_ERROR_MSG("Invalid db strings");
}
