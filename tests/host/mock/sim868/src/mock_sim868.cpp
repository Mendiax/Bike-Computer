
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <cstddef>
#include <thread>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

// my includes
#include "mock_sim868.hpp"
#include "pico/time.h"
#include "traces.h"
#include "mock_uart.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
Mock_SIM868::Mock_SIM868()
{
    this->stop = 0;
    this->urat_thread = NULL;
}
Mock_SIM868::~Mock_SIM868()
{
    this->stop = 1;
    this->urat_thread->join();
    delete this->urat_thread;
}

static void* thread_kernel(void* args);

void Mock_SIM868::run()
{
    if(urat_thread)
    {
        this->stop = 1;
        this->urat_thread->join();
        delete this->urat_thread;
    }
    this->stop = 0;
    this->urat_thread = new std::thread ([&]() {
            thread_kernel(nullptr);
        });
}

void* Mock_SIM868::thread_kernel(void* args)
{
    while (this->stop == 0)
    {
        auto msg = get_msg_from_pico();
        if(msg != "" && msg != "\r\n")
        {
            // PRINT("msg" << msg << "; " << msg.length());
            if(msg == "AT")
            {
                send_to_pico("\r\nOK\r\n");
            }
            else if(msg == "AT+CGNSINF")
            {
                send_to_pico(build_gps_response(1, 1,
                                                get_current_time().c_str(),
                                                53.0, 39.1, 350, 10, 9, 8, 7)
                                 .c_str());
            }
            else if(msg == "AT+CGNSPWR=1")
            {
                send_to_pico("AT+CGNSPWR=1: \r\nOK\r\n");
            }
            else if(msg == "AT+CBC")
            {
                send_to_pico("AT+CBC: 0,100,4.2\r\nOK\r\n");
            }
            else
            {
                TRACE_ABNORMAL(TRACE_HOST, "Unknown AT command:%s", msg.c_str());
                send_to_pico("Unknown: \r\nOK\r\n");
            }
        }
        else
        {
            sleep_ms(100);
        }
    }
    return NULL;
}

std::string Mock_SIM868::build_gps_response(
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
       << speedOverGround
       << ",courseOverGround,fixMode,...,hdop,pdop,vdop,...,"
       << gnssSatellitesInView << "," << gnssSatellitesUsed << ","
       << glonassSatellitesUsed
       << ",...,cnoMax,hpa,vpa\r\nOK\r\n";

    return ss.str();
}

std::string Mock_SIM868::get_current_time()
{
    const auto currentTime = std::chrono::system_clock::now();
    const  std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
    std::tm timeInfo;
    localtime_r(&currentTimeT, &timeInfo);

    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%Y%m%d%H%M%S.000");

    return ss.str();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
