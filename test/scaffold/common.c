
#include <time/time.h>
#include <devicemap.h>

uint8_t DMAP_MAXREGS = DMAP_MAX_REGISTERS;
ucdm_register_t ucdm_register[DMAP_MAX_REGISTERS];
uint8_t ucdm_acctype[DMAP_MAX_REGISTERS];

void setUp(void) {
    // setup stuff here
}

void tearDown(void) {
    // clean stuff up here
}

// One-time initialization. If the initialization is 
// expected to be reentrant or reversible, consider using
// setUp and tearDown instead. 
void libinit(void){
    tm_init(0);
}
