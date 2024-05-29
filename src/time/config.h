

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

/**@}*/ 


#endif

