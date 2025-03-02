#include "UniversalData.hpp"

#include <string>

UniversalData::UniversalData(bool value) :
    _type(UniversalDataType::Bool),
    _data(value) {}

UniversalData::UniversalData(int value) :
    _type(UniversalDataType::Int),
    _data(value) {}

UniversalData::UniversalData(double value) :
    _type(UniversalDataType::Double),
    _data(value) {}

UniversalData::UniversalData(std::string value) :
    _type(UniversalDataType::String),
    _data(std::move(value)) {}

UniversalData::UniversalData(const std::string_view value) :
    _type(UniversalDataType::String),
    _data(std::string{ value }) {}

UniversalData::UniversalData(const UniversalData& other) :
    _type(other._type),
    _data(other._data) {}

UniversalData& UniversalData::operator=(const UniversalData& other) {
    if (this == &other)
        return *this;

    _type = other._type;
    _data = other._data;

    return *this;
}

bool UniversalData::operator==(const UniversalData& other) const {
    if (_type != other._type)
        return false;

    return _data == other._data;
}

UniversalDataType UniversalData::GetType() const { return _type; }

template<>
bool UniversalData::Get<>() const {
    return std::get<bool>(_data);
}

template<>
int UniversalData::Get<>() const {
    return std::get<int>(_data);
}

template<>
double UniversalData::Get<>() const {
    return std::get<double>(_data);
}

template<>
std::string UniversalData::Get<>() const {
    return std::move(std::get<std::string>(_data));
}
