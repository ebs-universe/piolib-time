

#include <stdio.h>
#include <unity.h>

#include <time/time.h>
#include <time/systick_handler.h>

#include "../scaffold/common.c"


#if (defined __linux__ || defined _WIN32)
void test_systick_basic(void) {
    TEST_ASSERT_EQUAL(0, tm_current);
    for (int i=0; i<10; i++){
        time_systick_handler();
    }
    TEST_ASSERT_EQUAL(10, tm_current);
}
#else
void test_systick_basic(void) {
    tm_system_t ts1, ts2;
    tm_current_time(&ts1);
    // TODO insert dumb delay here, probably using something in platform?
    tm_current_time(&ts2);
    TEST_ASSERT_NOT_EQUAL(ts1, ts2);
    TEST_ASSERT_GREATER_THAN(ts2, ts1);
}
#endif

void test_systick_read(void) {
    tm_system_t ts1;
    tm_current_time(&ts1);
    TEST_ASSERT_EQUAL(tm_current, ts1);
    tm_current = 46949234482390;
    tm_current_time(&ts1);
    TEST_ASSERT_EQUAL(46949234482390, ts1);
    tm_current = 0;
}

int main( int argc, char **argv) {
    tm_init(0);
    UNITY_BEGIN();
    RUN_TEST(test_systick_read);
    RUN_TEST(test_systick_basic);
    UNITY_END();
}
