#include "UniversalData.hpp"

#include <cassert>
#include <string>

bool operator==(const UniversalData& lhs, const UniversalData& rhs) {
    if (lhs.index() != rhs.index())
        return false;

    if (std::holds_alternative<std::nullopt_t>(lhs))
        return std::holds_alternative<std::nullopt_t>(rhs);
    else if (std::holds_alternative<bool>(lhs))
        return std::get<bool>(lhs) == std::get<bool>(rhs);
    else if (std::holds_alternative<bool>(lhs))
        return std::get<bool>(lhs) == std::get<bool>(rhs);
    else if (std::holds_alternative<int>(lhs))
        return std::get<int>(lhs) == std::get<int>(rhs);
    else if (std::holds_alternative<double>(lhs))
        return std::get<double>(lhs) == std::get<double>(rhs);
    else if (std::holds_alternative<std::string>(lhs))
        return std::get<std::string>(lhs) == std::get<std::string>(rhs);

    assert(false);
}
