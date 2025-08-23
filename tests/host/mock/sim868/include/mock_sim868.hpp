#ifndef __SIM868_MOCK_SIM868_HPP__
#define __SIM868_MOCK_SIM868_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <atomic>
#include <thread>
#include <random>
#include <string>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
class Mock_SIM868{

private:
    std::thread* urat_thread;
    std::atomic_int stop;
    float current_lat;
    float current_lon;
    float current_alt;
    float current_speed;
    int current_sats_view;
    int current_sats_used;
    int current_glonass_used;
    std::mt19937 rng;
    std::normal_distribution<float> rand_pos_delta;     // For lat/lon
    std::normal_distribution<float> rand_alt_delta;     // For altitude
    std::normal_distribution<float> rand_speed_delta;   // For speed
    std::uniform_int_distribution<int> rand_sats_delta; // For satellites

public:
    Mock_SIM868();
    ~Mock_SIM868();

    /**
    Run a thread that simulate sim. Connect to uart and send
    response to AT commands.
    */
    void run();

    /**
    gps will use data from file to simulate data
    TODO implement
    */
    void set_gps_data(const char* csv_file_path);

private:
    void* thread_kernel(void* args);

    std::string build_gps_response(
        const int gnssRunStatus,
        const int fixStatus,
        const std::string& utcDateTime,
        const float latitude,
        const float longitude,
        const int mslAltitude,
        const float speedOverGround,
        const int gnssSatellitesInView,
        const int gnssSatellitesUsed,
        const int glonassSatellitesUsed);
    std::string get_current_time();
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
