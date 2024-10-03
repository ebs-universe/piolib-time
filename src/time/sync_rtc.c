

#include <hal/uc/rtc.h>
#include "sync.h"

tm_real_t rtc_buffer;


void tm_sync_rtc_wcb(void){
    ;
}

uint8_t tm_sync_current_to_rtc(void){
    critical_enter();
    tm_rtime_from_stime((tm_system_t *)&tm_current, &rtc_buffer);
    critical_exit();
    return rtc_write(&rtc_buffer, &tm_sync_rtc_wcb);
}

void tm_sync_rtc_rcb(void){
    critical_enter();
    tm_stime_from_rtime(&rtc_buffer, (tm_system_t *)&tm_current);
    critical_exit();
}

uint8_t tm_sync_current_from_rtc(void){
    return rtc_read(&rtc_buffer, &tm_sync_rtc_rcb);
}
