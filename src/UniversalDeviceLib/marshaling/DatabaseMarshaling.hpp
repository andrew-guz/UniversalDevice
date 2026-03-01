#pragma once

#include <string>
#include <vector>

#include "Event.hpp"

struct Device;
struct Scenario;
struct MotionRelayValue;
struct RelayValue;
struct ThermometerValue;
struct UniversalValue;

template<typename T>
void FromDbStrings(const std::vector<std::string>& dbStrings, T& object) = delete;

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, Device& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, Event& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, Scenario& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, MotionRelayValue& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, RelayValue& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, ThermometerValue& object);

template<>
void FromDbStrings(const std::vector<std::string>& dbStrings, UniversalValue& object);
