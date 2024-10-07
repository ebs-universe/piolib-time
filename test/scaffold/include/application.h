
/**
 * @name Application Systick and Time Configuration
 */
/**@{*/   
    #define APP_ENABLE_SYSTICK         0
    #define APP_ENABLE_TIME_CRON       1
    #define APP_ENABLE_TIME_SYNC       1
    #define APP_ENABLE_RTC             0  
    #define APP_ENABLE_RTC_1HZ_OUT     1
    #define APP_EXPOSE_TIME_UCDM       1
/**@}*/ 

// TODO RTC can be mocked using a native hal?
