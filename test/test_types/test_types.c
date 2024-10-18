

#include <unity.h>

#include <time/time.h>
#include <platform/debug.h>

#include <scaffold.h>

void test_stime(void) {
    TEST_ASSERT_EQUAL(sizeof(tm_system_t), 8);
    TEST_ASSERT_EQUAL(sizeof(tm_sdelta_t), 8);
    tm_system_t buffer = 423313231;
    uint32_t unixts;
    unixts = buffer;
    TEST_ASSERT_EQUAL(unixts, 423313231);
    TEST_ASSERT_EQUAL(buffer, 423313231);
    tm_clear_stime(&buffer);
    TEST_ASSERT_EQUAL(buffer, 0);

    tm_sdelta_t delta = 1000;
    TEST_ASSERT_EQUAL(delta, 1000);
    tm_clear_sdelta(&delta);
    TEST_ASSERT_EQUAL(delta, 0);
}

void test_tm_cmp_stime_t1_less_than_t2(void) {
    tm_system_t t1 = 1000;  // 1 second
    tm_system_t t2 = 2000;  // 2 seconds
    TEST_ASSERT_EQUAL_INT8(-1, tm_cmp_stime(&t1, &t2));
}

void test_tm_cmp_stime_t1_greater_than_t2(void) {
    tm_system_t t1 = 3000;  // 3 seconds
    tm_system_t t2 = 2000;  // 2 seconds
    TEST_ASSERT_EQUAL_INT8(1, tm_cmp_stime(&t1, &t2));
}

void test_tm_cmp_stime_t1_equal_t2(void) {
    tm_system_t t1 = 2000;  // 2 seconds
    tm_system_t t2 = 2000;  // 2 seconds
    TEST_ASSERT_EQUAL_INT8(0, tm_cmp_stime(&t1, &t2));
}

void test_tm_cmp_stime_t1_negative_less_than_t2(void) {
    tm_system_t t1 = -1000;  // -1 second
    tm_system_t t2 = 1000;   // 1 second
    TEST_ASSERT_EQUAL_INT8(-1, tm_cmp_stime(&t1, &t2));
}

void test_tm_cmp_stime_t1_negative_greater_than_t2(void) {
    tm_system_t t1 = -1000;  // -1 second
    tm_system_t t2 = -2000;  // -2 seconds
    TEST_ASSERT_EQUAL_INT8(1, tm_cmp_stime(&t1, &t2));
}

void test_rtime_size(void) {
    TEST_ASSERT_EQUAL(sizeof(tm_real_t), 10);
    TEST_ASSERT_EQUAL(sizeof(tm_rdelta_t), 8);
}

void test_rtime_init(void) {
    tm_real_t time = { .century = 20, .year = 23, .month = 10, .date = 5, .hours = 12, .minutes = 30, .seconds = 45, .millis = 500 };

    TEST_ASSERT_EQUAL_UINT8(20, time.century);
    TEST_ASSERT_EQUAL_UINT8(23, time.year);
    TEST_ASSERT_EQUAL_UINT8(10, time.month);
    TEST_ASSERT_EQUAL_UINT8(5, time.date);
    TEST_ASSERT_EQUAL_UINT8(12, time.hours);
    TEST_ASSERT_EQUAL_UINT8(30, time.minutes);
    TEST_ASSERT_EQUAL_UINT8(45, time.seconds);
    TEST_ASSERT_EQUAL_UINT16(500, time.millis);

    tm_clear_rtime(&time);

    TEST_ASSERT_EQUAL_UINT8(0, time.century);
    TEST_ASSERT_EQUAL_UINT8(0, time.year);
    TEST_ASSERT_EQUAL_UINT8(0, time.month);
    TEST_ASSERT_EQUAL_UINT8(0, time.date);
    TEST_ASSERT_EQUAL_UINT8(0, time.hours);
    TEST_ASSERT_EQUAL_UINT8(0, time.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, time.seconds);
    TEST_ASSERT_EQUAL_UINT16(0, time.millis);
} 

void test_rtime_max(void) {
    tm_real_t time = { .century = 99, .year = 99, .month = 12, .date = 31, .hours = 23, .minutes = 59, .seconds = 59, .millis = 999 };

    TEST_ASSERT_EQUAL_UINT8(99, time.century);
    TEST_ASSERT_EQUAL_UINT8(99, time.year);
    TEST_ASSERT_EQUAL_UINT8(12, time.month);
    TEST_ASSERT_EQUAL_UINT8(31, time.date);
    TEST_ASSERT_EQUAL_UINT8(23, time.hours);
    TEST_ASSERT_EQUAL_UINT8(59, time.minutes);
    TEST_ASSERT_EQUAL_UINT8(59, time.seconds);
    TEST_ASSERT_EQUAL_UINT16(999, time.millis);
}

void test_rtime_default(void) {
    tm_real_t time = {0}; 

    TEST_ASSERT_EQUAL_UINT8(0, time.century);
    TEST_ASSERT_EQUAL_UINT8(0, time.year);
    TEST_ASSERT_EQUAL_UINT8(0, time.month);
    TEST_ASSERT_EQUAL_UINT8(0, time.date);
    TEST_ASSERT_EQUAL_UINT8(0, time.hours);
    TEST_ASSERT_EQUAL_UINT8(0, time.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, time.seconds);
    TEST_ASSERT_EQUAL_UINT16(0, time.millis);
}

void test_rdelta_init(void) {
    tm_rdelta_t delta = { .sgn = 0, .seconds = 10, .minutes = 5, .hours = 2, .days = 1, .millis = 500 };

    TEST_ASSERT_EQUAL_UINT8(0, delta.sgn);
    TEST_ASSERT_EQUAL_UINT8(10, delta.seconds);
    TEST_ASSERT_EQUAL_UINT8(5, delta.minutes);
    TEST_ASSERT_EQUAL_UINT8(2, delta.hours);
    TEST_ASSERT_EQUAL_UINT16(1, delta.days);
    TEST_ASSERT_EQUAL_UINT16(500, delta.millis);

    tm_clear_rdelta(&delta);

    TEST_ASSERT_EQUAL_UINT8(0, delta.sgn);
    TEST_ASSERT_EQUAL_UINT8(0, delta.seconds);
    TEST_ASSERT_EQUAL_UINT8(0, delta.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, delta.hours);
    TEST_ASSERT_EQUAL_UINT16(0, delta.days);
    TEST_ASSERT_EQUAL_UINT16(0, delta.millis);
}

void test_rdelta_negative(void) {
    tm_rdelta_t delta = { .sgn = 1, .seconds = 59, .minutes = 59, .hours = 23, .days = 365, .millis = 999 };

    TEST_ASSERT_EQUAL_UINT8(1, delta.sgn);
    TEST_ASSERT_EQUAL_UINT8(59, delta.seconds);
    TEST_ASSERT_EQUAL_UINT8(59, delta.minutes);
    TEST_ASSERT_EQUAL_UINT8(23, delta.hours);
    TEST_ASSERT_EQUAL_UINT16(365, delta.days);
    TEST_ASSERT_EQUAL_UINT16(999, delta.millis);
}

void test_rdelta_default(void) {
    tm_rdelta_t delta = {0}; 

    TEST_ASSERT_EQUAL_UINT8(0, delta.sgn);
    TEST_ASSERT_EQUAL_UINT8(0, delta.seconds);
    TEST_ASSERT_EQUAL_UINT8(0, delta.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, delta.hours);
    TEST_ASSERT_EQUAL_UINT16(0, delta.days);
    TEST_ASSERT_EQUAL_UINT16(0, delta.millis);
}

void test_tm_check_invalid_rtime_valid_time(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 12, .date = 31, .hours = 23, .minutes = 59, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(0, result);  // Should return 0 for a valid time
}

void test_tm_check_invalid_rtime_invalid_century(void) {
    tm_real_t rtime = { .century = 25, .year = 22, .month = 12, .date = 31, .hours = 23, .minutes = 59, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(1, result);  // Should return 1 for invalid century
}

void test_tm_check_invalid_rtime_invalid_year(void) {
    tm_real_t rtime = { .century = 21, .year = 100, .month = 12, .date = 31, .hours = 23, .minutes = 59, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(2, result);  // Should return 2 for invalid year
}

void test_tm_check_invalid_rtime_invalid_month(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 13, .date = 31, .hours = 23, .minutes = 59, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(3, result);  // Should return 3 for invalid month
}

void test_tm_check_invalid_rtime_invalid_date(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 12, .date = 32, .hours = 23, .minutes = 59, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(4, result);  // Should return 4 for invalid date
}

void test_tm_check_invalid_rtime_invalid_hours(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 12, .date = 31, .hours = 24, .minutes = 59, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(5, result);  // Should return 5 for invalid hours
}

void test_tm_check_invalid_rtime_invalid_minutes(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 12, .date = 31, .hours = 23, .minutes = 60, .seconds = 59, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(6, result);  // Should return 6 for invalid minutes
}

void test_tm_check_invalid_rtime_invalid_seconds(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 12, .date = 31, .hours = 23, .minutes = 59, .seconds = 60, .millis = 999 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(7, result);  // Should return 7 for invalid seconds
}

void test_tm_check_invalid_rtime_invalid_millis(void) {
    tm_real_t rtime = { .century = 21, .year = 22, .month = 12, .date = 31, .hours = 23, .minutes = 59, .seconds = 59, .millis = 1000 };
    uint8_t result = tm_check_invalid_rtime(&rtime);
    TEST_ASSERT_EQUAL_UINT8(8, result);  // Should return 8 for invalid milliseconds
}


int main( int argc, char **argv) {
    init();
    UNITY_BEGIN();
    RUN_TEST(test_stime);
    RUN_TEST(test_tm_cmp_stime_t1_less_than_t2);
    RUN_TEST(test_tm_cmp_stime_t1_greater_than_t2);
    RUN_TEST(test_tm_cmp_stime_t1_equal_t2);
    RUN_TEST(test_tm_cmp_stime_t1_negative_less_than_t2);
    RUN_TEST(test_tm_cmp_stime_t1_negative_greater_than_t2);
    RUN_TEST(test_rtime_size);
    RUN_TEST(test_rtime_init);
    RUN_TEST(test_rtime_default);
    RUN_TEST(test_rtime_max);
    RUN_TEST(test_rdelta_init);
    RUN_TEST(test_rdelta_default);
    RUN_TEST(test_rdelta_negative);
    RUN_TEST(test_tm_check_invalid_rtime_valid_time);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_century);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_year);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_month);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_date);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_hours);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_minutes);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_seconds);
    RUN_TEST(test_tm_check_invalid_rtime_invalid_millis);
    UNITY_END();
}
