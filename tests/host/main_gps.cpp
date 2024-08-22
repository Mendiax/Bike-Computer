// pico includes
#include <pico/stdlib.h>

// c/c++ includes
// my includes
#include "traces.h"
#include "display_actor.hpp"

int main()
{
    //traces setup
    traces_init();
    TRACE_DEBUG(0, TRACE_MAIN, "Main start\n");
    PRINT("SYSTEM START");

    Sensor_Data sensors;
    Session_Data session;

    auto gui = Gui::get_gui(&sensors, &session);
    gui->go_next();
    while (true) {
        gui->render();
        gui->refresh();
        display::display();
        sleep_ms(50);
    }
}


