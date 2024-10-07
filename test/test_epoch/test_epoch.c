#include <unity.h>
#include <time/time.h>
#include "../scaffold/common.c"

// TODO 
// Test change of epoch, calculation of internal epoch offset, 
// epoch change handlers, etc

int main( int argc, char **argv) {
    tm_init(0);
    UNITY_BEGIN();
    UNITY_END();
}
