#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Defines.h"

struct Settings
{
    int _servicePort = DEFAULT_SERVICE_PORT;

    int _clientPort = DEFAULT_CLIENT_PORT;

    static Settings ReadSettings();
}; 

#endif //_SETTINGS_H_
