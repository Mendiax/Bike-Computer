// pico includes
#include <pico/stdlib.h>
#include <string.h>

// c/c++ includes
// my includes
#include "traces.h"
#include "core_setup.h"
#include "mock_sim868.hpp"

int main()
{
    //traces setup
    traces_init();
    TRACE_DEBUG(0, TRACE_MAIN, "Main start\n");
    PRINT("SYSTEM START");

    Mock_SIM868 mock_sim868;
    mock_sim868.run();

    core_setup();
}


