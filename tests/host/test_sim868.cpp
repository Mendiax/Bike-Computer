// #include "traces.h"
#include "pico/time.h"
#include "pico_test.hpp"
#include "sim868/gps.hpp"
#include "traces.h"
#include <atomic>
#include <cassert>
#include <cmath>
#include <stdio.h>
#include <pico/stdlib.h>
#include <iostream>
#include <thread>
#include "sim868/interface.hpp"
#include "mock_uart.hpp"
#include <sstream>

static std::atomic_int stop;

std::string buildATCommand(
    const int gnssRunStatus,
    const int fixStatus,
    const std::string& utcDateTime,
    const float latitude,
    const float longitude,
    const int mslAltitude,
    const float speedOverGround,
    const int gnssSatellitesInView,
    const int gnssSatellitesUsed,
    const int glonassSatellitesUsed)
{
    std::stringstream ss;
    ss << "AT+CGNSINF\r\n: "
       << gnssRunStatus << "," << fixStatus << "," << utcDateTime << ","
       << latitude << "," << longitude << "," << mslAltitude << ","
       << speedOverGround <<
       ",courseOverGround,fixMode,...,hdop,pdop,vdop,...,"
       << gnssSatellitesInView << "," << gnssSatellitesUsed << ","
       << glonassSatellitesUsed
       << ",...,cnoMax,hpa,vpa\r\nOK\r\n";

    return ss.str();
}

void sim868_emulator()
{
    // std::string at_cmd_pass[] = {
    //     "AT",
    //     "AT+CGNSPWR=1",
    //     "AT+CGNSPWR=0"
    // };
    std::string line;
    while (stop == 0)
    {
        auto msg = get_msg_from_pico();
        if(msg != "" && msg != "\r\n")
        {
            PRINT("msg" << msg << "; " << msg.length());
            if(msg == "AT")
            {
                PRINT("OK");
                send_to_pico("OK\r\n");
            }
            else if(msg == "AT+CGNSINF")
            {
                send_to_pico(buildATCommand(1,1,"20220812130030.000",53.0, 39.1,350,10,9,8,7).c_str());
            }
            else
            {
                send_to_pico("AT+CGNSPWR=1: \r\nOK\r\n");
            }
        }
    }
}

TEST_CASE_SUB_FUNCTION_DEF(uart_test)
{
    std::thread sim868_thread(sim868_emulator);
    sim868::boot();


    while (sim868::gps::fetch_data() == false) {
        sleep_ms(100);
    }
    float speed;
    sim868::gps::get_speed(speed);
    PICO_TEST_CHECK_EQ(speed, 10);

    stop++;
    sim868_thread.join();
}


TEST_CASE_FUNCTION(tc_sim868)
{
    TEST_CASE_SUB_FUNCTION(uart_test);
}

int main()
{
    traces_init();
    // TRACES_ON(TRACE_SPEED, 1); // speed update callback
    return tc_sim868().asserts_failed;
}