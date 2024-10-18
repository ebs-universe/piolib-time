

#include "unity_config.h"

#ifndef PIO_NATIVE

#include <hal/uc.h>


void unityOutputStart() {}


void unityOutputChar(char c) {
    while (!uart_reqlock(APP_BCIF_INTFNUM, 1, 0x00)){
        uart_send_trigger(APP_BCIF_INTFNUM);
    }
    uart_putc(APP_BCIF_INTFNUM, c, 0x00, 0x00); 
}


void unityOutputFlush() {
    uart_send_trigger(APP_BCIF_INTFNUM);
}


void unityOutputComplete() {}

#endif