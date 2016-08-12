


#include <ucdm/ucdm.h>
#include <time/time.h>
#include <time/sync.h>
#include"time_iface.h"

#define UCDM_TIME_SYSTEM_ADDRESS         UCDM_TIME_BASE_ADDRESS
#define UCDM_TIME_EPOCH_ADDRESS         (UCDM_TIME_BASE_ADDRESS  + (sizeof(tm_system_t) / 2) + (sizeof(tm_system_t) % 2 != 0))
#define UCDM_TIME_SYNC_HANDLER_ADDRESS  (UCDM_TIME_EPOCH_ADDRESS + (sizeof(tm_real_t) / 2)   + (sizeof(tm_real_t) % 2 != 0))

void app_tm_init(void)
{
    tm_init(UCDM_TIME_BASE_ADDRESS);
    tm_sync_init(UCDM_TIME_SYNC_HANDLER_ADDRESS);
}


void systick_init(void){
    timer_init(APP_SYSTICK_TIMER_INTFNUM);
    timer_set_prescaler(APP_SYSTICK_TIMER_INTFNUM, TIMER_PRESCALER_DIV8);
    timer_set_top(APP_SYSTICK_TIMER_INTFNUM, 
                  ((uC_TIMER_DEFAULT_CLKSOURCE_FREQ / 8 ) / APP_SYSTICK_FREQ_Hz )
    );
    timer_enable_int_top(APP_SYSTICK_TIMER_INTFNUM);
    timer_set_mode(APP_SYSTICK_TIMER_INTFNUM, TIMER_MODE_PERIODIC);
}


#if APP_ENABLE_RTC == 1
    uint8_t time_get_rtc_time(tm_real_t * rtime_target){
        return 0;
    }
#endif
