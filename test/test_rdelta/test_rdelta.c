#include <unity.h>
#include <time/time.h> 
#include <scaffold.h>


void test_tm_apply_rdelta_positive_delta(void) {
    tm_system_t t = 3600000;  // 1 hour in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 500, .seconds = 30, .minutes = 1, .hours = 0, .days = 0 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 1 hour + 1 minute + 30 seconds + 500 ms
    TEST_ASSERT_EQUAL_INT64(3690500, t);  // 3600000 + 60000 + 30000 + 500
}

void test_tm_apply_rdelta_negative_delta(void) {
    tm_system_t t = 3600000;  // 1 hour in milliseconds
    tm_rdelta_t delta = { .sgn = 1, .millis = 500, .seconds = 30, .minutes = 0, .hours = 0, .days = 0 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 1 hour - 30 seconds - 500 ms
    TEST_ASSERT_EQUAL_INT64(3569500, t);  // 3600000 - 30000 - 500
}

void test_tm_apply_rdelta_days(void) {
    tm_system_t t = 86400000;  // 1 day in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 0, .seconds = 0, .minutes = 0, .hours = 0, .days = 2 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 1 day + 2 days
    TEST_ASSERT_EQUAL_INT64(3 * 86400000, t);  // 86400000 * 3
}

void test_tm_apply_rdelta_large_negative_delta(void) {
    tm_system_t t = 3600000;  // 1 hour in milliseconds
    tm_rdelta_t delta = { .sgn = 1, .millis = 0, .seconds = 0, .minutes = 0, .hours = 3, .days = 1 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 1 hour - 3 hours - 1 day
    TEST_ASSERT_EQUAL_INT64(-93600000, t);  // 3600000 - 3*3600000 - 86400000
}

void test_tm_apply_rdelta_month_overflow(void) {
    tm_system_t t = 253402300799000;  // Equivalent to 9999-12-31 23:59:59 UTC in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 1000, .seconds = 0, .minutes = 0, .hours = 0, .days = 0 };

    tm_apply_rdelta(&t, &delta);

    // Expected: Transition into the year 10000, overflow check
    TEST_ASSERT_EQUAL_INT64(253402300800000, t);  // 9999-12-31 23:59:59 + 1 sec
}

void test_tm_apply_rdelta_leap_year(void) {
    tm_system_t t = 1582934400000;  // 2020-02-29 00:00:00 UTC (leap year) in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 0, .seconds = 0, .minutes = 0, .hours = 24, .days = 0 };

    tm_apply_rdelta(&t, &delta);

    // Expected: Transition to the next day (2020-03-01 00:00:00)
    TEST_ASSERT_EQUAL_INT64(1583020800000, t);  // 24 hours later, March 1st
}

void test_tm_apply_rdelta_end_of_month_overflow(void) {
    tm_system_t t = 1609459199000;  // 2020-12-31 23:59:59 UTC in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 1000, .seconds = 0, .minutes = 0, .hours = 0, .days = 0 };

    tm_apply_rdelta(&t, &delta);

    // Expected: Transition to the next year (2021-01-01 00:00:00)
    TEST_ASSERT_EQUAL_INT64(1609459200000, t);  // 2021-01-01 00:00:00
}

void test_tm_apply_rdelta_hour_overflow(void) {
    tm_system_t t = 3600000;  // 1 hour in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 0, .seconds = 0, .minutes = 0, .hours = 23, .days = 0 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 24 hours (day overflow), timestamp should move by exactly 24 hours
    TEST_ASSERT_EQUAL_INT64(86400000, t);  // 24 hours in milliseconds
}

void test_tm_apply_rdelta_negative_end_of_month(void) {
    tm_system_t t = 1612137600000;  // 2021-02-01 00:00:00 UTC in milliseconds
    tm_rdelta_t delta = { .sgn = 1, .millis = 0, .seconds = 0, .minutes = 0, .hours = 0, .days = 1 };

    tm_apply_rdelta(&t, &delta);

    // Expected: Go back to 2021-01-31 00:00:00 UTC
    TEST_ASSERT_EQUAL_INT64(1612051200000, t);  // 2021-01-31 00:00:00 UTC
}

void test_tm_apply_rdelta_mixed_delta(void) {
    tm_system_t t = 3600100;  // 1 hour + 100 ms in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 950, .seconds = 45, .minutes = 1, .hours = 0, .days = 0 };
    
    tm_apply_rdelta(&t, &delta);

    // Expected: 1 hour + 1 minute + 45 seconds + 1050 ms (100 ms + 950 ms)
    TEST_ASSERT_EQUAL_INT64(3706050, t);  // 3600100 + 60000 + 45000 + 950
}

void test_tm_apply_rdelta_mixed_positive_delta(void) {
    tm_system_t t = 1609459200000;  // 2021-01-01 00:00:00 UTC in milliseconds
    tm_rdelta_t delta = { .sgn = 0, .millis = 500, .seconds = 30, .minutes = 45, .hours = 1, .days = 1 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 2021-01-02 01:45:30.500 UTC
    TEST_ASSERT_EQUAL_INT64(1609551930500, t);  // 1 day, 1 hour, 45 min, 30 sec, 500 ms later
}

void test_tm_apply_rdelta_mixed_negative_delta(void) {
    tm_system_t t = 1609551930500;  // 2021-01-02 01:45:30.500 UTC in milliseconds
    tm_rdelta_t delta = { .sgn = 1, .millis = 500, .seconds = 30, .minutes = 45, .hours = 1, .days = 1 };

    tm_apply_rdelta(&t, &delta);

    // Expected: Back to 2021-01-01 00:00:00 UTC
    TEST_ASSERT_EQUAL_INT64(1609459200000, t);  // 1 day, 1 hour, 45 min, 30 sec, 500 ms earlier
}

void test_tm_apply_rdelta_large_mixed_positive_delta(void) {
    tm_system_t t = 0;  // 1970-01-01 00:00:00 UTC (Epoch start)
    tm_rdelta_t delta = { .sgn = 0, .millis = 250, .seconds = 15, .minutes = 30, .hours = 12, .days = 365 };

    tm_apply_rdelta(&t, &delta);

    // Expected: 1971-01-01 12:30:15.250 UTC
    TEST_ASSERT_EQUAL_INT64(31581015250, t);  // 365 days, 12 ho1urs, 30 min, 15 sec, 250 ms later
}

void test_tm_apply_rdelta_large_mixed_negative_delta(void) {
    tm_system_t t = 31557615250;  // 1971-01-01 6:00:15.250 UTC in milliseconds
    tm_rdelta_t delta = { .sgn = 1, .millis = 250, .seconds = 15, .minutes = 0, .hours = 6, .days = 365 };

    tm_apply_rdelta(&t, &delta);

    // Expected: Back to 1970-01-01 00:00:00 UTC
    TEST_ASSERT_EQUAL_INT64(0, t);  // Back to Unix Epoch start
}

int main( int argc, char **argv) {
    init();

    UNITY_BEGIN();
    RUN_TEST(test_tm_apply_rdelta_positive_delta);
    RUN_TEST(test_tm_apply_rdelta_negative_delta);
    RUN_TEST(test_tm_apply_rdelta_days);
    RUN_TEST(test_tm_apply_rdelta_large_negative_delta);
    RUN_TEST(test_tm_apply_rdelta_month_overflow);
    RUN_TEST(test_tm_apply_rdelta_leap_year);
    RUN_TEST(test_tm_apply_rdelta_end_of_month_overflow);    
    RUN_TEST(test_tm_apply_rdelta_hour_overflow);
    RUN_TEST(test_tm_apply_rdelta_negative_end_of_month);
    RUN_TEST(test_tm_apply_rdelta_mixed_delta);
    RUN_TEST(test_tm_apply_rdelta_mixed_positive_delta);
    RUN_TEST(test_tm_apply_rdelta_mixed_negative_delta);
    RUN_TEST(test_tm_apply_rdelta_large_mixed_positive_delta);
    RUN_TEST(test_tm_apply_rdelta_large_mixed_negative_delta);
    UNITY_END();
}
