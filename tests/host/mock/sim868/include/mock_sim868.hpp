#ifndef __SIM868_MOCK_SIM868_HPP__
#define __SIM868_MOCK_SIM868_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <atomic>
#include <thread>
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
