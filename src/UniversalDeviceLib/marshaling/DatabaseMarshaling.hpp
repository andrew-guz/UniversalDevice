#pragma once

#include <string>
#include <vector>

struct Device;
struct ExtendedComponentDescription;
struct ExtendedMotionRelayCurrentState;
struct ExtendedRelayCurrentState;
struct ExtendedThermometerCurrentValue;
struct ExtendedUniversalDeviceCurrentValues;
struct RelayValue;
struct ThermometerValue;

template<typename T>
void FromDbStrings(const std::vector<std::string>& dbStrings, T& object) = delete;

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, Device& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedComponentDescription& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedMotionRelayCurrentState& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedRelayCurrentState& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedThermometerCurrentValue& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ExtendedUniversalDeviceCurrentValues& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, RelayValue& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ThermometerValue& object);
