#ifndef _SETUP_HELPER_H_
#define _SETUP_HELPER_H_

#include "Defines.h"

void setupHelper()
{
    //check that variables are set
    #ifndef UUID
        static_assert(false, "No UUID")
    #endif
    static_assert(sizeof(UUID) > 0, "No UUID");
    #ifndef PASSWORD_MAIN
        static_assert(false, "No PASSWORD_MAIN")
    #endif
    static_assert(sizeof(PASSWORD_MAIN) > 0, "No PASSWORD_MAIN");
    #ifndef PASSWORD_ADDITIONAL
        static_assert(false, "No PASSWORD_ADDITIONAL")
    #endif
    static_assert(sizeof(PASSWORD_ADDITIONAL) > 0, "No PASSWORD_ADDITIONAL");
}

#endif //_SETUP_HELPER_H_
