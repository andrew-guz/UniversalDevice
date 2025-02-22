#pragma once

#include <optional>
#include <string>
#include <variant>

using UniversalData = std::variant<std::nullopt_t, bool, int, double, std::string>;

bool operator==(const UniversalData& lhs, const UniversalData& rhs);
