#pragma once

#include "Uuid.hpp"

class Constants final {
public:
    // login
    static constexpr std::string_view LoginService = "service";
    static constexpr std::string_view LoginDevice = "device";

    // predefined ids
    static const Uuid PredefinedIdTimer;
    static const Uuid PredefinedIdClient;
    static const Uuid PredefinedIdSunrise;
    static const Uuid PredefinedIdSunset;
};
