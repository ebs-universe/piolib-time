

#include "test_time.h"
#include <time/sync.h>
#include <time/cron.h>
#include "bsp/drivers/led/led.h"

cron_job_t gltoggle;
cron_job_t rlon;
cron_job_t rloff;

tm_sdelta_t dot = {100, 0, 0};
tm_sdelta_t dash = {0, 1, 0};

static void green_led_toggle(void){
    led_toggle(BOARD_GREEN_LED_SELECTOR);
}

static void red_led_on(void){
    led_on(BOARD_RED_LED_SELECTOR);
}

static void red_led_off(void){
    led_off(BOARD_RED_LED_SELECTOR);
}

void time_test_init(void){
    tm_sync_request_host();
    tm_cron_create_job_abs(&rlon, &red_led_on, &tm_current, &dash);
    tm_cron_create_job_rel(&rloff, &red_led_off, &dot, &dash);
    tm_cron_create_job_abs(&gltoggle, &green_led_toggle, &tm_current, &dot);
    
}                        
