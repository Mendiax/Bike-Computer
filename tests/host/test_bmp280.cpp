// #include "traces.h"
#include <cassert>
#include <stdio.h>
#include <pico/stdlib.h>
#include "bc_test.h"
#include "bmp280.hpp"
#include "bmp280_raw.hpp"
#include <iostream>




int test_bmp280_basic()
{
    bmp280_init();

    for(int i = 0; i < 100; i++)
    {
        bmp280_update_data();
        float temp = bmp280_get_temp();
        float press = bmp280_get_press();
        std::cout << temp << " " << press << std::endl;
        BC_CHECK(temp > 0);
        BC_CHECK(press > 0);
    }
    return 0;
}

int main()
{
    BC_TEST(test_bmp280_basic);
    BC_TEST_END();

    return 0;
}
