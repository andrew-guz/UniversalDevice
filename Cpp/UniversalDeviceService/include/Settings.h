#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Defines.h"

struct Settings
{
    int _port = 8080;

    static Settings ReadSettings();
}; 

#endif //_SETTINGS_H_
