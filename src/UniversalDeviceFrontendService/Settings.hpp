#pragma once

#include "Defines.hpp"

struct Settings {
    int _servicePort = DEFAULT_SERVICE_PORT;

    int _frontendPort = DEFAULT_FRONTEND_PORT;

    static Settings ReadSettings();
};
