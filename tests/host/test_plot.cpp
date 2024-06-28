#include "pico_test.hpp"
#include "traces.h"
#include <pico/stdlib.h>
#include "views/screenfunc/plot.h"


TEST_CASE_SUB_FUNCTION_DEF(test_map)
{
    PICO_TEST_CHECK_VERBAL(map(5, 0, 10, 0.0f, 1.0f) == 0.5f, "Expected %f, got %f", 0.5f, map(5, 0, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(10, 0, 10, 0.0f, 1.0f) == 1.0f, "Expected %f, got %f", 1.0f, map(10, 0, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(0, 0, 10, 0.0f, 1.0f) == 0.0f, "Expected %f, got %f", 0.0f, map(0, 0, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(5, 0, 10, 10, 20) == 15, "Expected %d, got %d", 15, map(5, 0, 10, 10, 20));

    // Test map function with negative numbers
    PICO_TEST_CHECK_VERBAL(map(-5, -10, 0, 0.0f, 1.0f) == 0.5f, "Expected %f, got %f", 0.5f, map(-5, -10, 0, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(-10, -10, 0, 0.0f, 1.0f) == 0.0f, "Expected %f, got %f", 0.0f, map(-10, -10, 0, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(0, -10, 0, 0.0f, 1.0f) == 1.0f, "Expected %f, got %f", 1.0f, map(0, -10, 0, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(-5, -10, 0, 10, 20) == 15, "Expected %d, got %d", 15, map(-5, -10, 0, 10, 20));

    // Test map function with mixed positive and negative numbers
    PICO_TEST_CHECK_VERBAL(map(-5, -10, 10, 0.0f, 1.0f) == 0.25f, "Expected %f, got %f", 0.25f, map(-5, -10, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(0, -10, 10, 0.0f, 1.0f) == 0.5f, "Expected %f, got %f", 0.5f, map(0, -10, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(10, -10, 10, 0.0f, 1.0f) == 1.0f, "Expected %f, got %f", 1.0f, map(10, -10, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(-10, -10, 10, 10, 20) == 10, "Expected %d, got %d", 10, map(-10, -10, 10, 10, 20));
    PICO_TEST_CHECK_VERBAL(map(5, -10, 10, 10.0, 20.0) == 17.5, "Expected %f, got %f", 17.5, map(5, -10, 10, 10.0, 20.0));

    // Test map function with values out of range
    PICO_TEST_CHECK_VERBAL(map(-15, -10, 10, 0.0f, 1.0f) == 0.0f, "Expected %f, got %f", 0.0f, map(-15, -10, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(15, -10, 10, 0.0f, 1.0f) == 1.0f, "Expected %f, got %f", 1.0f, map(15, -10, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(-20, -10, 0, 10, 20) == 10, "Expected %d, got %d", 10, map(-20, -10, 0, 10, 20));
    PICO_TEST_CHECK_VERBAL(map(5, 0, 10, 10, 20) == 15, "Expected %d, got %d", 15, map(5, 0, 10, 10, 20));

    // Test map function with edge cases
    PICO_TEST_CHECK_VERBAL(map(10, 10, 10, 0.0f, 1.0f) == 1.0f, "Expected %f, got %f", 1.0f, map(10, 10, 10, 0.0f, 1.0f));
    PICO_TEST_CHECK_VERBAL(map(0, 0, 0, 0.0f, 1.0f) == 1.0f, "Expected %f, got %f", 1.0f, map(0, 0, 0, 0.0f, 1.0f));
}


TEST_CASE_FUNCTION(tc_tools)
{
    TEST_CASE_SUB_FUNCTION(test_map);
}

int main()
{
    traces_init();
    return tc_tools().asserts_failed;
}