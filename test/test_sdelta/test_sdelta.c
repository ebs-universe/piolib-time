#include <unity.h>
#include <time/time.h> 
#include <scaffold.h>


void test_tm_get_sdelta_positive_delta(void) {
    tm_system_t t1 = 1000;  // 1 second (in ms)
    tm_system_t t2 = 2000;  // 2 seconds (in ms)
    tm_sdelta_t sdelta;

    tm_get_sdelta(&t1, &t2, &sdelta);
    TEST_ASSERT_EQUAL_INT64(1000, sdelta);  // Expected delta: 1 second (1000 ms)
}

void test_tm_get_sdelta_negative_delta(void) {
    tm_system_t t1 = 2000;  // 2 seconds (in ms)
    tm_system_t t2 = 1000;  // 1 second (in ms)
    tm_sdelta_t sdelta;

    tm_get_sdelta(&t1, &t2, &sdelta);
    TEST_ASSERT_EQUAL_INT64(-1000, sdelta);  // Expected delta: -1 second (-1000 ms)
}

void test_tm_get_sdelta_zero_delta(void) {
    tm_system_t t1 = 1500;  // 1.5 seconds (in ms)
    tm_system_t t2 = 1500;  // 1.5 seconds (in ms)
    tm_sdelta_t sdelta;

    tm_get_sdelta(&t1, &t2, &sdelta);
    TEST_ASSERT_EQUAL_INT64(0, sdelta);  // Expected delta: 0 ms
}

void test_tm_get_sdelta_large_positive_delta(void) {
    tm_system_t t1 = 1000000;  
    tm_system_t t2 = 3600000000;  
    tm_sdelta_t sdelta;

    tm_get_sdelta(&t1, &t2, &sdelta);
    TEST_ASSERT_EQUAL_INT64(3599000000, sdelta);  
}

void test_tm_get_sdelta_large_negative_delta(void) {
    tm_system_t t1 = 3600000000;  
    tm_system_t t2 = 1000000;  
    tm_sdelta_t sdelta;

    tm_get_sdelta(&t1, &t2, &sdelta);
    TEST_ASSERT_EQUAL_INT64(-3599000000, sdelta);  
}

void test_tm_apply_sdelta_positive_delta(void) {
    tm_system_t t = 1000000;  // 1000 second (in ms)
    tm_sdelta_t sdelta = 500000;  // 500 seconds (in ms)

    tm_apply_sdelta(&t, &sdelta);
    TEST_ASSERT_EQUAL_INT64(1500000, t);  // Expected result: 1500 seconds 
}

void test_tm_apply_sdelta_negative_delta(void) {
    tm_system_t t = 2000000;  // 2000 seconds (in ms)
    tm_sdelta_t sdelta = -1000000;  // -1000 second (in ms)

    tm_apply_sdelta(&t, &sdelta);
    TEST_ASSERT_EQUAL_INT64(1000000, t);  // Expected result: 1000 second
}

void test_tm_apply_sdelta_zero_delta(void) {
    tm_system_t t = 1500000;  // 1500 seconds (in ms)
    tm_sdelta_t sdelta = 0;  // 0 ms

    tm_apply_sdelta(&t, &sdelta);
    TEST_ASSERT_EQUAL_INT64(1500000, t);  // Expected result: 1500 seconds 
}

void test_tm_apply_sdelta_large_positive_delta(void) {
    tm_system_t t = 1000000;  
    tm_sdelta_t sdelta = 3600000000;  

    tm_apply_sdelta(&t, &sdelta);
    TEST_ASSERT_EQUAL_INT64(3601000000, t);  
}

void test_tm_apply_sdelta_large_negative_delta(void) {
    tm_system_t t = 3600000000;  
    tm_sdelta_t sdelta = -3600000000;  

    tm_apply_sdelta(&t, &sdelta);
    TEST_ASSERT_EQUAL_INT64(0, t); 
}


int main( int argc, char **argv) {
    init();

    UNITY_BEGIN();
    RUN_TEST(test_tm_get_sdelta_positive_delta);
    RUN_TEST(test_tm_get_sdelta_negative_delta);
    RUN_TEST(test_tm_get_sdelta_zero_delta);
    RUN_TEST(test_tm_get_sdelta_large_positive_delta);
    RUN_TEST(test_tm_get_sdelta_large_negative_delta);
    RUN_TEST(test_tm_apply_sdelta_positive_delta);
    RUN_TEST(test_tm_apply_sdelta_negative_delta);
    RUN_TEST(test_tm_apply_sdelta_zero_delta);
    RUN_TEST(test_tm_apply_sdelta_large_positive_delta);
    RUN_TEST(test_tm_apply_sdelta_large_negative_delta);
    UNITY_END();
}
