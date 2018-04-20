


#include "watchdog.h"
#include "bsp/hal/uc/core.h"
#include <time/cron.h>

static cron_job_t _watchdog_service;
static tm_sdelta_t _watchdog_period = {0, 4, 0};


void watchdog_service_init(void){
    watchdog_init();
    tm_cron_create_job_rel(&_watchdog_service, &watchdog_clear, 
                           &_watchdog_period, &_watchdog_period);
}


