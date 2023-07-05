#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "pico/time.h"
#include "tests/test_cases.h"
#include "bc_test.h"
#include "pico_test.hpp"
#include "traces.h"




int main()
{
    bi_decl(bi_program_description("FreeRTOS test"));

    printf("main()\n");

    stdio_init_all();
    traces_init();

    pico_test_start();


    tc_rtos_basic();

    // only one tc per bin for rtos!!!
    while (true) {
        sleep_ms(500);
        printf("blink\n");
    }
}

