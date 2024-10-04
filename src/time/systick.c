

#include "config.h"
#include "systick.h"

#if TIME_USE_CORE_SYSTICK
#include <hal/uc/core.h>
#endif

#if TIME_USE_TIMER_SYSTICK
#include <hal/uc/timer.h>
#endif

void tm_systick_init(void) {
    #if TIME_USE_CORE_SYSTICK
        core_systick_start();
    #elif TIME_USE_TIMER_SYSTICK
        timer_init(APP_SYSTICK_TIMER_INTFNUM);
        timer_set_prescaler(
            APP_SYSTICK_TIMER_INTFNUM, 
            APP_SYSTICK_TIMER_PRESCALER
        );
        timer_set_top(
            APP_SYSTICK_TIMER_INTFNUM, 
            APP_SYSTICK_TIMER_TOP
        );
        timer_set_mode(
            APP_SYSTICK_TIMER_INTFNUM, 
            TIMER_MODE_PERIODIC
        );
        timer_enable_int_top(APP_SYSTICK_TIMER_INTFNUM);
    #endif
}
