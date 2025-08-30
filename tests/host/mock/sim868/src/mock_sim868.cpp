
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
#include <random>

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
Mock_SIM868::Mock_SIM868() :
    stop(0),
    urat_thread(NULL),
    current_lat(53.0),
    current_lon(39.1),
    current_alt(350),
    current_speed(10.0),
    current_sats_view(12),
    current_sats_used(8),
    current_glonass_used(4),
    rng(std::random_device{}()),
    rand_pos_delta(0.0001, 0.1),    // ~10m range for position
    rand_alt_delta(0.0, 0.5),       // 0.5m standard deviation for altitude
    rand_speed_delta(0.0, 0.5),     // 0.2 km/h standard deviation for speed
    rand_sats_delta(-1, 1)          // -1, 0, or +1 for satellite count changes
{
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
                // Update position with small random changes
                current_lat += rand_pos_delta(rng);
                current_lon += rand_pos_delta(rng);

                // Update altitude (keep it reasonable: 300-400m)
                current_alt += rand_alt_delta(rng);
                current_alt = std::min(400.0f, std::max(300.0f, current_alt));

                // Update speed (keep it non-negative and reasonable: 0-60 km/h)
                current_speed += rand_speed_delta(rng);
                current_speed = std::min(60.0f, std::max(0.0f, current_speed));

                // Update satellite counts (keep within reasonable ranges)
                current_sats_view += rand_sats_delta(rng);
                current_sats_view = std::min(15, std::max(8, current_sats_view));

                current_sats_used += rand_sats_delta(rng);
                current_sats_used = std::min(current_sats_view, std::max(4, current_sats_used));

                current_glonass_used += rand_sats_delta(rng);
                current_glonass_used = std::min(8, std::max(2, current_glonass_used));

                send_to_pico(build_gps_response(1, 1,
                                                get_current_time().c_str(),
                                                current_lat, current_lon,
                                                static_cast<int>(current_alt),
                                                current_speed,
                                                current_sats_view,
                                                current_sats_used,
                                                current_glonass_used)
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
