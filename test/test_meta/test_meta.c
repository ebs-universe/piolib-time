

#include <unity.h>
#include <time/time.h>
#include <ucdm/descriptor.h>

#include "../scaffold/common.c"

void test_descriptor_version(void) {
    tm_install_descriptor();
    char readbuffer[20];
    descriptor_custom_t * desc = descriptor_find(DESCRIPTOR_TAG_LIBVERSION);
    TEST_ASSERT_NOT_NULL(desc);
    TEST_ASSERT_EQUAL(6, desc->length);
    descriptor_read(desc, &readbuffer);
    TEST_ASSERT_EQUAL_STRING("0.2.1", readbuffer);
    return;
}

void test_descriptor_epoch(void) {
    tm_real_t epochbuffer;
    descriptor_custom_t * desc = descriptor_find(DESCRIPTOR_TAG_TIME_EPOCH);
    TEST_ASSERT_NOT_NULL(desc);
    TEST_ASSERT_EQUAL(sizeof(tm_real_t), desc->length);
    descriptor_read(desc, &epochbuffer);
    TEST_ASSERT_EQUAL(19, epochbuffer.century);
    TEST_ASSERT_EQUAL(70, epochbuffer.year);
    TEST_ASSERT_EQUAL(1,  epochbuffer.month);
    TEST_ASSERT_EQUAL(1,  epochbuffer.date);
    TEST_ASSERT_EQUAL(0,  epochbuffer.hours);
    TEST_ASSERT_EQUAL(0,  epochbuffer.minutes);
    TEST_ASSERT_EQUAL(0,  epochbuffer.seconds);
    return;
}

int main( int argc, char **argv) {
    libinit();
    UNITY_BEGIN();
    RUN_TEST(test_descriptor_epoch);
    RUN_TEST(test_descriptor_version);
    UNITY_END();
}
