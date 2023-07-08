#include "pico_test.hpp"
#include "pico/stdlib.h"
#include <stdio.h>

TEST_CASE_FUNCTION(tc_basic_interface)
{
    printf("Hello, world!\n");

    PICO_TEST_ASSERT(1 == 1);
}