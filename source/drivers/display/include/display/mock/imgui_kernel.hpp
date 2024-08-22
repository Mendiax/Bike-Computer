#ifndef __DISPLAY_IMGUI_KERNEL_HPP__
#define __DISPLAY_IMGUI_KERNEL_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// c/c++ includes
#include <mutex>

// my includes
#include "display/display_config.hpp"
#include "display/driver.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Holds data for imgui. Main program loads here data.
 *
 */
struct Imgui_Display_Data{
    std::mutex mtx_buffer;
    display::DisplayColor imgui_display_buffer[DISPLAY_PIXEL_COUNT];
    volatile bool stop;
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#
/**
 * @brief kernel for running display
 *
 * @param args pointer to Imgui_Display_Data
 */
void imgui_thread_kernel(void* args);

// #-------------------------------#
// |  global function definitions  |
// #-------------------------------#

#endif
