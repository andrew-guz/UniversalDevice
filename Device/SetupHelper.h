#ifndef _SETUP_HELPER_H_
#define _SETUP_HELPER_H_

#include <assert.h>

#include "Defines.h"

void CheckDefines() {
// check that variables are set
#ifndef DEVICE_UUID
    static_assert(false, "No DEVICE_UUID")
#endif
        static_assert(sizeof(DEVICE_UUID) > 0, "No DEVICE_UUID");
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
}

#endif //_SETUP_HELPER_H_
