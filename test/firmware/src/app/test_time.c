

#include "test_time.h"
#include <time/sync.h>
#include <time/cron.h>
#include "bsp/drivers/led/led.h"

#include "bsp/hal/uc/timer.h"

cron_job_t gltoggle;
cron_job_t rlon;
cron_job_t rloff;

tm_sdelta_t dot = {100, 0, 0};
tm_sdelta_t dash = {0, 1, 0};
tm_system_t start;


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
    led_init(BOARD_GREEN_LED_SELECTOR);
    led_off(BOARD_GREEN_LED_SELECTOR);
    
    led_init(BOARD_RED_LED_SELECTOR);
    led_off(BOARD_RED_LED_SELECTOR);
        
    tm_sync_request_host();
    tm_cron_create_job_abs(&rlon, &red_led_on, &tm_current, &dash);
    tm_cron_create_job_rel(&rloff, &red_led_off, &dot, &dash);
    
    tm_current_time(&start);
    tm_apply_sdelta(&start, &dash);
    tm_cron_create_job_abs(&gltoggle, &green_led_toggle, &start, &dot);
}                        
