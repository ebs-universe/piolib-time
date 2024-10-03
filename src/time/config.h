

#ifndef TIME_CONFIG_H
#define TIME_CONFIG_H

#define TIME_VERSION       "0.1.0"

#include <application.h>

/**
 * @name Time Epoch Configuration Definitions
 * 
 * Configuration of the default epoch of the core time library. 
 * The default is to use the UNIX epoch, and this should be fine 
 * for most applications. The application can override these in 
 * its application.h using the EBS_ #defines. 
 * 
 */
/**@{*/ 

#ifdef EBS_TIME_EPOCH_CENTURY
    #define TIME_DEFAULT_EPOCH_CENTURY      EBS_TIME_EPOCH_CENTURY
#else
    #define TIME_DEFAULT_EPOCH_CENTURY      19
#endif

#ifdef EBS_TIME_EPOCH_YEAR
    #define TIME_DEFAULT_EPOCH_YEAR         EBS_TIME_EPOCH_YEAR
#else
    #define TIME_DEFAULT_EPOCH_YEAR         70
#endif

#ifdef EBS_TIME_EPOCH_MONTH
    #define TIME_DEFAULT_EPOCH_MONTH        EBS_TIME_EPOCH_MONTH
#else
    #define TIME_DEFAULT_EPOCH_MONTH        1
#endif

#ifdef EBS_TIME_EPOCH_DATE
    #define TIME_DEFAULT_EPOCH_DATE         EBS_TIME_EPOCH_DATE
#else
    #define TIME_DEFAULT_EPOCH_DATE         1
#endif

#ifdef EBS_DESCRIPTOR_TIME_EPOCH
    #define DESCRIPTOR_TAG_TIME_EPOCH       EBS_DESCRIPTOR_TIME_EPOCH
#else
    #define DESCRIPTOR_TAG_TIME_EPOCH       0x02
#endif

#if defined EBS_TIME_ENABLE_SYNC
    #define TIME_ENABLE_SYNC                EBS_TIME_ENABLE_SYNC
#elif defined APP_ENABLE_TIME_SYNC
    #define TIME_ENABLE_SYNC                APP_ENABLE_TIME_SYNC
#else
    #define TIME_ENABLE_SYNC                0
#endif

#if defined APP_ENABLE_RTC
    #define TIME_ENABLE_SYNC_RTC            APP_ENABLE_RTC
#else
    #define TIME_ENABLE_SYNC_RTC            0
#endif


#if defined EBS_TIME_ENABLE_CRON
    #define TIME_ENABLE_CRON                EBS_TIME_ENABLE_CRON
#elif defined APP_ENABLE_TIME_CRON
    #define TIME_ENABLE_CRON                APP_ENABLE_TIME_CRON
#else
    #define TIME_ENABLE_CRON                0
#endif


/**@}*/ 


#endif

