#define UNITY_INCLUDE_CONFIG_H
#include "../lib/hdc.c" /* needed to unit test static functions */
#include "unity.h"

void setUp()
{
}

void tearDown()
{
}

void test_hdc_dot_product()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0 };
    double b[] = { 5.0, 6.0, 7.0, 8.0 };
    int len = 4;
    TEST_ASSERT_EQUAL_FLOAT(70.0, dot_product(a, b, len));
}

void test_hdc_norm()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0 };
    int len = 4;
    TEST_ASSERT_EQUAL_FLOAT(5.4772, norm(a, len));
}

void test_hdc_entrywise_product()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0 };
    double b[] = { 5.0, 6.0, 7.0, 8.0 };
    double c[] = { 5.0, 12.0, 21.0, 32.0 };
    double d[4];
    int len = 4;
    entrywise_product(d, a, b, len);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(c, d, len);
}

void test_hdc_entrywise_sum()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0 };
    double b[] = { 5.0, 6.0, 7.0, 8.0 };
    double c[] = { 6.0, 8.0, 10.0, 12.0 };
    double d[4];
    int len = 4;
    entrywise_sum(d, a, b, len);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(c, d, len);
}

void test_hdc_cos_angle()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0 };
    double b[] = { 5.0, 6.0, 7.0, 8.0 };
    int len = 4;
    TEST_ASSERT_EQUAL_FLOAT(0.96886, cos_angle(a, b, len));
}

void test_hdc_circ_shift()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0 };
    double b[] = { 4.0, 1.0, 2.0, 3.0 };
    int len = 4;
    circ_shift(a, 4);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(a, b, len);
}

int main(int argc, char* argv[])
{
    UNITY_BEGIN();
    RUN_TEST(test_hdc_dot_product);
    RUN_TEST(test_hdc_norm);
    RUN_TEST(test_hdc_entrywise_product);
    RUN_TEST(test_hdc_entrywise_sum);
    RUN_TEST(test_hdc_cos_angle);
    RUN_TEST(test_hdc_circ_shift);
    return UNITY_END();
}
