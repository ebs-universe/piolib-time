#include <unity.h>
#include <time/time.h>
#include <scaffold.h>


void test_tm_stime_from_rtime_basic_conversion(void) {
    tm_real_t rtime = {0};
    tm_system_t stime = 0;

    // Test for 1st Jan 1970 00:00:00 (Unix epoch start)
    rtime.century = 19;
    rtime.year = 70;  
    rtime.month = 1;  
    rtime.date = 1;
    rtime.hours = 0;
    rtime.minutes = 0;
    rtime.seconds = 0;
    rtime.millis = 0;
    
    tm_stime_from_rtime(&rtime, &stime);
    TEST_ASSERT_EQUAL_INT64(0, stime);  // Should be Unix epoch (0 ms)

    // Test for 1st Jan 2024 00:00:00
    rtime.century = 20;
    rtime.year = 24;  // 2024
    rtime.month = 1;
    rtime.date = 1;
    rtime.hours = 0;
    rtime.minutes = 0;
    rtime.seconds = 0;
    rtime.millis = 0;

    tm_stime_from_rtime(&rtime, &stime);
    // Verify stime is correct for Unix timestamp 1704067200000 (milliseconds)
    TEST_ASSERT_EQUAL_INT64(1704067200000LL, stime);
}

void test_tm_stime_from_rtime_leap_year(void) {
    tm_real_t rtime = {0};
    tm_system_t stime = 0;

    // Test leap year: 29th Feb 2024
    rtime.century = 20;
    rtime.year = 24;  // 2024
    rtime.month = 2;   // February
    rtime.date = 29;   // Leap day
    rtime.hours = 0;
    rtime.minutes = 0;
    rtime.seconds = 0;
    rtime.millis = 0;

    tm_stime_from_rtime(&rtime, &stime);
    // Verify stime for 29th Feb 2024 00:00:00, 1709164800000 ms since Unix epoch
    TEST_ASSERT_EQUAL_INT64(1709164800000LL, stime);

    // Test for 1st Mar 2024 00:00:00
    rtime.month = 3;
    rtime.date = 1;
    stime = 0;
    tm_stime_from_rtime(&rtime, &stime);
    // Verify stime for 1st Mar 2024 00:00:00, should account for leap day
    TEST_ASSERT_EQUAL_INT64(1709251200000LL, stime);
}

void test_tm_stime_from_rtime_negative_timestamps(void) {
    tm_real_t rtime = {0};
    tm_system_t stime = 0;

    // Test for 31st Dec 1969 23:59:59, just before Unix epoch
    rtime.century = 19;
    rtime.year = 69;   // 1969
    rtime.month = 12;  // December
    rtime.date = 31;
    rtime.hours = 23;
    rtime.minutes = 59;
    rtime.seconds = 59;
    rtime.millis = 0;

    tm_stime_from_rtime(&rtime, &stime);
    // Should return -1000 ms, just before the Unix epoch
    TEST_ASSERT_EQUAL_INT64(-1000LL, stime);

    // Test for 1st Jan 1960 00:00:00, a date well before Unix epoch
    rtime.century = 19;
    rtime.year = 60;   // 1960
    rtime.month = 1;
    rtime.date = 1;
    rtime.hours = 0;
    rtime.minutes = 0;
    rtime.seconds = 0;
    rtime.millis = 0;

    tm_stime_from_rtime(&rtime, &stime);
    // Verify stime for 1st Jan 1960, -315619200000 ms
    TEST_ASSERT_EQUAL_INT64(-315619200000LL, stime);
}

void test_tm_stime_from_rtime_end_of_month(void) {
    tm_real_t rtime = {0};
    tm_system_t stime = 0;

    // Test for 31st Jan 2024 23:59:59
    rtime.century = 20;
    rtime.year = 24;  // 2024
    rtime.month = 1;   // January
    rtime.date = 31;
    rtime.hours = 23;
    rtime.minutes = 59;
    rtime.seconds = 59;
    rtime.millis = 999; // Almost at the end of the day

    tm_stime_from_rtime(&rtime, &stime);
    // Verify correct timestamp in ms for 31st Jan 2024 23:59:59.999
    TEST_ASSERT_EQUAL_INT64(1706745599999LL, stime);

    // Test for 29th Feb 2024 23:59:59 (leap year)
    rtime.month = 2;
    rtime.date = 29;

    tm_stime_from_rtime(&rtime, &stime);
    // Verify timestamp accounts for leap year ending Feb 29th
    TEST_ASSERT_EQUAL_INT64(1709251199999LL, stime);

    // Test for 31st Dec 2024 23:59:59
    rtime.century = 20;
    rtime.year = 24;  // 2024
    rtime.month = 12;  // December
    rtime.date = 31;
    rtime.hours = 23;
    rtime.minutes = 59;
    rtime.seconds = 59;

    tm_stime_from_rtime(&rtime, &stime);
    // Verify correct timestamp for 31st Dec 2024 23:59:59.999
    TEST_ASSERT_EQUAL_INT64(1735689599999LL, stime);
}

void test_tm_stime_from_rtime_fractional_seconds(void) {
    tm_real_t rtime = {0};
    tm_system_t stime = 0;

    // Test for 1st Jan 2024 12:30:30.123
    rtime.century = 20;
    rtime.year = 24;  // 2024
    rtime.month = 1;
    rtime.date = 1;
    rtime.hours = 12;
    rtime.minutes = 30;
    rtime.seconds = 30;
    rtime.millis = 123;

    tm_stime_from_rtime(&rtime, &stime);
    // Verify correct timestamp with milliseconds
    TEST_ASSERT_EQUAL_INT64(1704112230123LL, stime);
}

void test_tm_sdelta_from_rdelta_basic_conversion(void) {
    tm_rdelta_t rdelta = {0};
    tm_sdelta_t sdelta = 0;

    // Test for 1 day
    rdelta.sgn = 0;
    rdelta.days = 1;
    rdelta.hours = 0;
    rdelta.minutes = 0;
    rdelta.seconds = 0;
    rdelta.millis = 0;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // 1 day = 24 * 60 * 60 * 1000 ms = 86400000 ms
    TEST_ASSERT_EQUAL_INT64(86400000, sdelta);

    // Test for 1 hour
    rdelta.days = 0;
    rdelta.hours = 1;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // 1 hour = 60 * 60 * 1000 ms = 3600000 ms
    TEST_ASSERT_EQUAL_INT64(3600000, sdelta);
}

void test_tm_sdelta_from_rdelta_with_milliseconds(void) {
    tm_rdelta_t rdelta = {0};
    tm_sdelta_t sdelta = 0;

    // Test for 1 second and 500 milliseconds
    rdelta.seconds = 1;
    rdelta.millis = 500;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // 1 second = 1000 ms, plus 500 ms
    TEST_ASSERT_EQUAL_INT64(1500, sdelta);

    // Test for 2 minutes, 10 seconds, and 250 ms
    rdelta.minutes = 2;
    rdelta.seconds = 10;
    rdelta.millis = 250;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // 2 * 60 * 1000 ms + 10 * 1000 ms + 250 ms = 130250 ms
    TEST_ASSERT_EQUAL_INT64(130250, sdelta);
}

void test_tm_sdelta_from_rdelta_large_values(void) {
    tm_rdelta_t rdelta = {0};
    tm_sdelta_t sdelta = 0;

    // Test for 365 days (1 year), 5 hours, 10 minutes, 15 seconds, and 100 ms
    rdelta.days = 365;
    rdelta.hours = 5;
    rdelta.minutes = 10;
    rdelta.seconds = 15;
    rdelta.millis = 100;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // (365 * 86400000) + (5 * 3600000) + (10 * 60000) + (15 * 1000) + 100 = total ms
    int64_t expected_sdelta = (365 * 86400000LL) + (5 * 3600000LL) + (10 * 60000LL) + (15 * 1000LL) + 100;
    TEST_ASSERT_EQUAL_INT64(expected_sdelta, sdelta);
}

void test_tm_sdelta_from_rdelta_negative_values(void) {
    tm_rdelta_t rdelta = {0};
    tm_sdelta_t sdelta = 0;

    // Test for negative 1 minute (should work with negative deltas if supported)
    rdelta.sgn = 1;
    rdelta.minutes = 1;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // -1 minute = -60000 ms
    TEST_ASSERT_EQUAL_INT64(-60000, sdelta);

    // Test for negative 2 days and 3 hours
    rdelta.minutes = 0;
    rdelta.days = 2;
    rdelta.hours = 3;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // (-2 * 86400000) + (-3 * 3600000) = -183600000 ms
    TEST_ASSERT_EQUAL_INT64(-183600000, sdelta);
}

void test_tm_sdelta_from_rdelta_zero_delta(void) {
    tm_rdelta_t rdelta = {0};
    tm_sdelta_t sdelta = 0;

    // All fields are zero
    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    TEST_ASSERT_EQUAL_INT64(0, sdelta); // Expected: 0 ms
}

void test_tm_sdelta_from_rdelta_boundary_conditions(void) {
    tm_rdelta_t rdelta = {0};
    tm_sdelta_t sdelta = 0;

    rdelta.days = 0;
    rdelta.hours = 23;
    rdelta.minutes = 59;
    rdelta.seconds = 59;
    rdelta.millis = 999;

    tm_sdelta_from_rdelta(&rdelta, &sdelta);
    // Expected result should be just under one day
    TEST_ASSERT_EQUAL_INT64(86399999, sdelta); // 23:59:59.999 = 86399999 ms
}

int main( int argc, char **argv) {
    init();
    UNITY_BEGIN();
    RUN_TEST(test_tm_stime_from_rtime_basic_conversion);
    RUN_TEST(test_tm_stime_from_rtime_leap_year);
    RUN_TEST(test_tm_stime_from_rtime_negative_timestamps);
    RUN_TEST(test_tm_stime_from_rtime_end_of_month);
    RUN_TEST(test_tm_stime_from_rtime_fractional_seconds);
    RUN_TEST(test_tm_sdelta_from_rdelta_basic_conversion);
    RUN_TEST(test_tm_sdelta_from_rdelta_with_milliseconds);
    RUN_TEST(test_tm_sdelta_from_rdelta_large_values);
    RUN_TEST(test_tm_sdelta_from_rdelta_negative_values);
    RUN_TEST(test_tm_sdelta_from_rdelta_zero_delta);
    RUN_TEST(test_tm_sdelta_from_rdelta_boundary_conditions);
    UNITY_END();
}
