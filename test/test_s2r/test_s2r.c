#include <unity.h>
#include <time/time.h>
#include "../scaffold/common.c"

// Helper function to validate tm_real_t
void assert_tm_real_equal(tm_real_t* expected, tm_real_t* actual) {
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->century, actual->century, "century");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->year, actual->year, "year");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->month, actual->month, "month");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->date, actual->date, "date");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->hours, actual->hours, "hours");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->minutes, actual->minutes, "minutes");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected->seconds, actual->seconds, "seconds");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected->millis, actual->millis, "millis");
}

void test_tm_rtime_from_stime_unix_epoch(void) {
    tm_system_t stime = 0;
    tm_real_t rtime = {0};

    tm_rtime_from_stime(&stime, &rtime);
    
    tm_real_t expected = {19, 70, 1, 1, 0, 0, 0, 0}; // 1970-01-01 00:00:00.000
    assert_tm_real_equal(&expected, &rtime);
}

void test_tm_rtime_from_stime_positive_timestamp(void) {
    tm_system_t stime = 1000000; // 1,000,000 ms after epoch
    tm_real_t rtime = {0};

    tm_rtime_from_stime(&stime, &rtime);
    
    tm_real_t expected = {19, 70, 1, 1, 0, 16, 40, 0}; // 1970-01-01 00:16:40.000
    assert_tm_real_equal(&expected, &rtime);
}

void test_tm_rtime_from_stime_negative_timestamp(void) {
    tm_system_t stime = -1000000; // -1,000,000 ms (before epoch)
    tm_real_t rtime = {0};

    tm_rtime_from_stime(&stime, &rtime);
    
    tm_real_t expected = {19, 69, 12, 31, 23, 43, 19, 0}; // 1969-12-31 23:43:19.000
    assert_tm_real_equal(&expected, &rtime);
}

void test_tm_rtime_from_stime_leap_year(void) {
    tm_system_t stime = 1582934400000; // 2020-02-29 00:00:00.000 (UTC)
    tm_real_t rtime = {0};

    tm_rtime_from_stime(&stime, &rtime);
    
    tm_real_t expected = {20, 20, 2, 29, 0, 0, 0, 0}; // 2020-02-29 00:00:00.000
    assert_tm_real_equal(&expected, &rtime);
}

void test_tm_rtime_from_stime_month_boundary(void) {
    tm_system_t stime = 1612137600000; // 2021-02-01 00:00:00.000 (UTC)
    tm_real_t rtime = {0};

    tm_rtime_from_stime(&stime, &rtime);
    
    tm_real_t expected = {20, 21, 2, 1, 0, 0, 0, 0}; // 2021-02-01 00:00:00.000
    assert_tm_real_equal(&expected, &rtime);
}

void test_tm_rtime_from_stime_end_of_year(void) {
    tm_system_t stime = 1703980800000; // 2023-12-31 00:00:00.000 (UTC)
    tm_real_t rtime = {0};

    tm_rtime_from_stime(&stime, &rtime);
    
    tm_real_t expected = {20, 23, 12, 31, 0, 0, 0, 0}; // 2023-12-31 00:00:00.000
    assert_tm_real_equal(&expected, &rtime);
}

void test_tm_rdelta_from_sdelta_zero_delta(void) {
    tm_sdelta_t sdelta = 0;
    tm_rdelta_t rdelta = {0};

    tm_rdelta_from_sdelta(&sdelta, &rdelta);
    TEST_ASSERT_EQUAL(0, rdelta.sgn);
    TEST_ASSERT_EQUAL_UINT16(0, rdelta.days);
    TEST_ASSERT_EQUAL_UINT8(0, rdelta.hours);
    TEST_ASSERT_EQUAL_UINT8(0, rdelta.minutes);
    TEST_ASSERT_EQUAL_UINT8(0, rdelta.seconds);
    TEST_ASSERT_EQUAL_UINT16(0, rdelta.millis);
}

void test_tm_rdelta_from_sdelta_positive_delta(void) {
    tm_sdelta_t sdelta = 90061001; // 1 day, 1 hour, 1 minute, 1 second, 1ms in ms
    tm_rdelta_t rdelta = {0};

    tm_rdelta_from_sdelta(&sdelta, &rdelta);
    TEST_ASSERT_EQUAL(0, rdelta.sgn);
    TEST_ASSERT_EQUAL_UINT16(1, rdelta.days);
    TEST_ASSERT_EQUAL_UINT8(1, rdelta.hours);
    TEST_ASSERT_EQUAL_UINT8(1, rdelta.minutes);
    TEST_ASSERT_EQUAL_UINT8(1, rdelta.seconds);
    TEST_ASSERT_EQUAL_UINT16(1, rdelta.millis);
}

void test_tm_rdelta_from_sdelta_negative_delta(void) {
    tm_sdelta_t sdelta = -90061001; // -1day. -1 hour, -1 minute, -1 second, -1 ms in ms
    tm_rdelta_t rdelta = {0};

    tm_rdelta_from_sdelta(&sdelta, &rdelta);
    TEST_ASSERT_EQUAL_MESSAGE(1, rdelta.sgn, "sign");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(1, rdelta.days, "days");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, rdelta.hours, "hours");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, rdelta.minutes, "minutes");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, rdelta.seconds, "seconds");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(1, rdelta.millis, "millis");
}

void test_tm_rdelta_from_sdelta_large_positive(void) {
    tm_sdelta_t sdelta = 5662223999999; // 65535 days - 1 ms
    tm_rdelta_t rdelta = {0};

    tm_rdelta_from_sdelta(&sdelta, &rdelta);
    TEST_ASSERT_EQUAL_MESSAGE(0, rdelta.sgn, "sign");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(65534, rdelta.days, "days");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(23, rdelta.hours, "hours");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(59, rdelta.minutes, "minutes");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(59, rdelta.seconds, "seconds");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(999, rdelta.millis, "millis");
}

void test_tm_rdelta_from_sdelta_large_negative(void) {
    tm_sdelta_t sdelta = -5662223999999; // 65535 days - 1 ms
    tm_rdelta_t rdelta = {0};

    tm_rdelta_from_sdelta(&sdelta, &rdelta);
    TEST_ASSERT_EQUAL_MESSAGE(1, rdelta.sgn, "sign");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(65534, rdelta.days, "days");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(23, rdelta.hours, "hours");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(59, rdelta.minutes, "minutes");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(59, rdelta.seconds, "seconds");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(999, rdelta.millis, "millis");
}


int main( int argc, char **argv) {
    tm_init(0);
    UNITY_BEGIN();
    RUN_TEST(test_tm_rtime_from_stime_unix_epoch);
    RUN_TEST(test_tm_rdelta_from_sdelta_zero_delta);
    RUN_TEST(test_tm_rdelta_from_sdelta_positive_delta);
    RUN_TEST(test_tm_rdelta_from_sdelta_negative_delta);
    RUN_TEST(test_tm_rdelta_from_sdelta_large_positive);
    RUN_TEST(test_tm_rdelta_from_sdelta_large_negative);
    RUN_TEST(test_tm_rtime_from_stime_positive_timestamp);
    // Not supported
    // RUN_TEST(test_tm_rtime_from_stime_negative_timestamp);
    RUN_TEST(test_tm_rtime_from_stime_leap_year);
    RUN_TEST(test_tm_rtime_from_stime_month_boundary);
    RUN_TEST(test_tm_rtime_from_stime_end_of_year);
    UNITY_END();
}
