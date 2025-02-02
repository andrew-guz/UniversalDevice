#ifndef _SETUP_HELPER_H_
#define _SETUP_HELPER_H_

#include <assert.h>

#include "Defines/Defines.h"

void CheckDefines() {
// check that variables are set
#ifndef DEVICE_UUID
    static_assert(false, "No DEVICE_UUID")
#endif
        static_assert(sizeof(DEVICE_UUID) > 0, "No DEVICE_UUID");

#ifndef DEVICE_NAME
    static_assert(false, "No DEVICE_NAME")
#endif
        static_assert(sizeof(DEVICE_NAME) > 0, "No DEVICE_NAME");

#ifndef SSID_MAIN
    static_assert(false, "No SSID_MAIN")
#endif
        static_assert(sizeof(SSID_MAIN) > 0, "No SSID_MAIN");

#ifndef SSID_ADDITIONAL
    static_assert(false, "No SSID_ADDITIONAL")
#endif
        static_assert(sizeof(SSID_ADDITIONAL) > 0, "No SSID_ADDITIONAL");

#ifndef PASSWORD_MAIN
    static_assert(false, "No PASSWORD_MAIN")
#endif
        static_assert(sizeof(PASSWORD_MAIN) > 0, "No PASSWORD_MAIN");

#ifndef PASSWORD_ADDITIONAL
    static_assert(false, "No PASSWORD_ADDITIONAL")
#endif
        static_assert(sizeof(PASSWORD_ADDITIONAL) > 0, "No PASSWORD_ADDITIONAL");

#ifndef AUTHORIZATION_STR
    static_assert(false, "No AUTHORIZATION_STR")
#endif
        static_assert(sizeof(AUTHORIZATION_STR) > 0, "No AUTHORIZATION_STR");

#ifndef OTA_PASSWORD
    static_assert(false, "No OTA_PASSWORD")
#endif
        static_assert(sizeof(OTA_PASSWORD) > 0, "No OTA_PASSWORD");

#if !defined HAS_THERMOMETER && !defined HAS_RELAY && !defined HAS_MOTION_RELAY
    static_assert(false, "No HAS_ type selection");
#endif
}

#endif //_SETUP_HELPER_H_
