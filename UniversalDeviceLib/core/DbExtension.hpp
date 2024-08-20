#pragma once

#include "Enums.hpp"
#include "Uuid.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class DbExtension final {
public:
    DbExtension() = delete;

    ~DbExtension() = default;

    template<typename T>
    static T CreateFromDbStrings(const std::vector<std::string>& dbStrings) {
        T t;
        t.FromDbStrings(dbStrings);
        return t;
    }

    template<typename T>
    static std::vector<T> CreateVectorFromDbStrings(const std::vector<std::vector<std::string>>& dbStringsVector) {
        std::vector<T> result;
        for (auto& dbStrings : dbStringsVector)
            result.push_back(CreateFromDbStrings<T>(dbStrings));
        return result;
    }

    template<typename T>
    static std::optional<T> FindValueByName(const std::vector<std::string>& dbStrings, std::string_view name) = delete;
};

template<>
std::optional<std::string> DbExtension::FindValueByName<std::string>(const std::vector<std::string>& dbStrings, std::string_view name);

template<>
std::optional<EventType> DbExtension::FindValueByName<EventType>(const std::vector<std::string>& dbStrings, std::string_view name);

template<>
std::optional<Uuid> DbExtension::FindValueByName<Uuid>(const std::vector<std::string>& dbStrings, std::string_view name);

template<>
std::optional<std::chrono::system_clock::time_point>
DbExtension::FindValueByName<std::chrono::system_clock::time_point>(const std::vector<std::string>& dbStrings, std::string_view name);

template<>
std::optional<bool> DbExtension::FindValueByName<bool>(const std::vector<std::string>& dbStrings, std::string_view name);

template<>
std::optional<int> DbExtension::FindValueByName<int>(const std::vector<std::string>& dbStrings, std::string_view name);

template<>
std::optional<float> DbExtension::FindValueByName<float>(const std::vector<std::string>& dbStrings, std::string_view name);
