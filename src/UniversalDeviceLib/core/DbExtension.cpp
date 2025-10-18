#include "DbExtension.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <optional>

#include <nlohmann/json.hpp>

#include "Marshaling.hpp"
#include "TimeHelper.hpp"
#include "Uuid.hpp"

namespace {
    std::optional<std::string> FindStringValueByName(const std::vector<std::string>& dbStrings, const std::string_view name) {
        auto iter = std::find(dbStrings.begin(), dbStrings.end(), name);
        if (iter != dbStrings.end()) {
            auto index = iter - dbStrings.begin();
            if (index % 2 == 0)
                return dbStrings.at(index + 1);
        }
        return std::nullopt;
    }
} // namespace

template<>
std::optional<std::string> DbExtension::FindValueByName<std::string>(const std::vector<std::string>& dbStrings, const std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return std::move(value.value());
    return std::nullopt;
}

template<>
std::optional<EventType> DbExtension::FindValueByName<EventType>(const std::vector<std::string>& dbStrings, const std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return EnumFromString<EventType>(value.value());
    return std::nullopt;
}
template<>
std::optional<DeviceType> DbExtension::FindValueByName<DeviceType>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return EnumFromString<DeviceType>(value.value());
    return std::nullopt;
}

template<>
std::optional<ActorType> DbExtension::FindValueByName<ActorType>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return ActorTypeFromString(value.value());
    return std::nullopt;
}

template<>
std::optional<Uuid> DbExtension::FindValueByName<Uuid>(const std::vector<std::string>& dbStrings, const std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return Uuid(value.value());
    return std::nullopt;
}

template<>
std::optional<std::chrono::system_clock::time_point>
DbExtension::FindValueByName<std::chrono::system_clock::time_point>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return TimeHelper::TimeFromInt((int64_t)std::stoll(value.value()));
    return std::nullopt;
}

template<>
std::optional<bool> DbExtension::FindValueByName<bool>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return std::atoi(value.value().c_str());
    return std::nullopt;
}

template<>
std::optional<int> DbExtension::FindValueByName<int>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return std::atoi(value.value().c_str());
    return std::nullopt;
}

template<>
std::optional<std::uint64_t> DbExtension::FindValueByName<std::uint64_t>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return std::atoll(value.value().c_str());
    return std::nullopt;
}

template<>
std::optional<float> DbExtension::FindValueByName<float>(const std::vector<std::string>& dbStrings, std::string_view name) {
    std::optional<std::string> value = FindStringValueByName(dbStrings, name);
    if (value.has_value())
        return std::atof(value.value().c_str());
    return std::nullopt;
}
