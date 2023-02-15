// #include "traces.h"
#include <stdio.h>
#include <pico/stdlib.h>
// #include "test.hpp"
#include "bc_test.h"
// #include "pico/pico_host_sdl.h"




int test_basic()
{
    sleep_ms(1000);
    BC_CHECK(1 == 1);
    return 0;
}

// #ifdef __cplusplus
// extern "C" {
// #endif

// // C-style function or data declarations or definitions

// #ifdef __cplusplus
// }
// #endif

int main()
{
    BC_TEST(test_basic);
    BC_TEST_END();
    sleep_ms(1000);
    return 0;
}
