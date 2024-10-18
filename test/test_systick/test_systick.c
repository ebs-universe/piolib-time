

#include <stdio.h>
#include <unity.h>

#include <time/time.h>
#include <time/systick_handler.h>
#include <platform/debug.h>
#include <scaffold.h>


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
    // TODO This needs to be implemented in platform/debug
    delay_ms(10); 
    tm_current_time(&ts2);
    TEST_ASSERT_NOT_EQUAL(ts1, ts2);
    TEST_ASSERT_GREATER_THAN(ts2, ts1);
}
#endif

void test_systick_read(void) {
    tm_system_t ts1, ts2;
    
    critical_enter();
    tm_current_time(&ts1);
    critical_exit();
    TEST_ASSERT_EQUAL(tm_current, ts1);
    
    critical_enter();
    tm_current_time(&ts2);
    tm_current = 46949234482390;
    tm_current_time(&ts1);
    critical_exit();
    TEST_ASSERT_EQUAL(46949234482390, ts1);
    
    tm_current = ts2;
}

int main( int argc, char **argv) {
    init();
    UNITY_BEGIN();
    RUN_TEST(test_systick_read);
    RUN_TEST(test_systick_basic);
    UNITY_END();
}
