#include "unity.h"

void setUp()
{
}

void tearDown()
{
}

void test_hdc()
{
    int a = 2;
    TEST_ASSERT_EQUAL(2, a);
}

int main(int argc, char* argv[])
{
    UNITY_BEGIN();
    RUN_TEST(test_hdc);
    return UNITY_END();
}
