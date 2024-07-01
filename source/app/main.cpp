// pico includes
#include <pico/stdlib.h>
#include <string.h>

// c/c++ includes
// my includes
#include "traces.h"
#include "core_setup.h"

int main()
{
    //traces setup
    traces_init();

    #ifdef BUILD_FOR_PICO
    // setup console
    sleep_ms(1000);
    stdio_init_all();
    sleep_ms(2000);
    #endif

    TRACE_DEBUG(0, TRACE_MAIN, "Main start\n");
    PRINT("SYSTEM START");

    core_setup();
}

