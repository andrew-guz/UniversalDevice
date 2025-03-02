#pragma once

#include <string>
#include <string_view>
#include <variant>

#include "Enums.hpp"

struct UniversalData final {
public:
    UniversalData(bool value);

    UniversalData(int value);

    UniversalData(double value);

    UniversalData(std::string value);

    UniversalData(std::string_view value);

    UniversalData(const UniversalData& other);

    UniversalData& operator=(const UniversalData& other);

    bool operator==(const UniversalData& other) const;

    UniversalDataType GetType() const;

    template<typename T>
    T Get() const = delete;

private:
    UniversalDataType _type = UniversalDataType::Bool;

    std::variant<bool, int, double, std::string> _data = false;
};

template<>
bool UniversalData::Get<>() const;

template<>
int UniversalData::Get<>() const;

template<>
double UniversalData::Get<>() const;

template<>
std::string UniversalData::Get<>() const;
