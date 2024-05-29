


#include <hal/uc/timer.h>
#include "application.h"
#include "systick.h"
#include "systick_handler.h"


void tm_systick_init(void) {
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
}
