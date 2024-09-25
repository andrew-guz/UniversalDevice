#pragma once

#include "Defines.hpp"

struct Settings {
    int _port = DEFAULT_SERVICE_PORT;

    static Settings ReadSettings();
};
