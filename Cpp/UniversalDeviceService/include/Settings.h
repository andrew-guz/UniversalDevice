#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Defines.h"

struct Settings {
    int _port = DEFAULT_SERVICE_PORT;

    static Settings ReadSettings();
};

#endif //_SETTINGS_H_
