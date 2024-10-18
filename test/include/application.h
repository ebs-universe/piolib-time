

#ifndef PIO_NATIVE
#include <scaffold_application.h>
#endif

#ifndef APP_UCDM_MAX_REGISTERS
#define APP_UCDM_MAX_REGISTERS              20
#endif

#ifdef APP_ENABLE_LIBVERSION_DESCRIPTORS
#undef APP_ENABLE_LIBVERSION_DESCRIPTORS
#endif

#define APP_ENABLE_LIBVERSION_DESCRIPTORS   0

#ifdef TIME_LIBVERSION_DESCRIPTOR
#undef TIME_LIBVERSION_DESCRIPTOR
#endif

#define TIME_LIBVERSION_DESCRIPTOR          1

/**
 * @name Application Systick and Time Configuration
 */
/**@{*/
    #ifndef APP_ENABLE_SYSTICK
    #define APP_ENABLE_SYSTICK         0
    #endif

    #ifndef APP_ENABLE_TIME_CRON
    #define APP_ENABLE_TIME_CRON       1
    #endif

    #ifndef APP_ENABLE_TIME_SYNC
    #define APP_ENABLE_TIME_SYNC       1
    #endif

    #ifndef APP_ENABLE_RTC 
    #define APP_ENABLE_RTC             0
    #endif

    #ifndef APP_EXPOSE_TIME_UCDM
    #define APP_EXPOSE_TIME_UCDM       1
    #endif
/**@}*/ 

// TODO RTC can be mocked using a native hal?
