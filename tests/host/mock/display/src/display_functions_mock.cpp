
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes
#include "pico/time.h"

// // external lib
// #include "imgui.h"
// #include "imgui_impl_sdl2.h"
// #include "imgui_impl_opengl2.h"
// #include <SDL.h>
// #include <SDL_opengl.h>

// c/c++ includes
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <mutex>
#include <thread>


// my includes
#include "display_functions_mock.hpp"
#include "traces.h"
#include "hw_functions.hpp"
// #include "speedometer/speedometer.hpp"
#include "imgui_kernel.hpp"
// #include "display/display_config.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#


// needs to be static because we imitate physical device
static Imgui_Display_Data imgui_display_data;
static std::thread* imgui_thread = nullptr;


// #------------------------------#
// | static functions declarations|
// #------------------------------#


// #------------------------------#
// | global function definitions  |
// #------------------------------#
// function from display driver
void display_init(void)
{
    // clean image buffer
    memset(imgui_display_data.imgui_display_buffer, 0, sizeof(imgui_display_data.imgui_display_buffer));
    Imgui_Display::start();
}

void display_sync(void)
{

}

// function from display driver
void display_display(uint8_t display_buffer[DISPLAY_BUFFER_SIZE])
{
    (void)display_buffer;
    std::unique_lock<std::mutex> lock(imgui_display_data.mtx_buffer);


    for(size_t idx = 0; idx < DISPLAY_PIXEL_COUNT; idx+=1)
    {
        struct pixel2
        {
            uint8_t rg;
            uint8_t br;
            uint8_t gb;

        };
        pixel2 *buffer_pixels = (pixel2 *)display_buffer;

        const uint_fast32_t index_pixels = idx >> 1;
        pixel2 *const pixel_p = &buffer_pixels[index_pixels];
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        if (idx & 1)
        {
            r = pixel_p->br & 0x0f;
            g = pixel_p->gb & 0xf0;
            b = pixel_p->gb & 0x0f;
        }
        else
        {
            r = pixel_p->rg & 0xf0;
            g = pixel_p->rg & 0x0f;
            b = pixel_p->br & 0xf0;
        }
        imgui_display_data.imgui_display_buffer[idx] = {r,g,b};
    }
}

void Imgui_Display::start(void)
{
    std::unique_lock<std::mutex> lock(imgui_display_data.mtx_buffer);
    ::imgui_thread = new std::thread(imgui_thread_kernel, &imgui_display_data);
}

void Imgui_Display::stop(void)
{
    std::unique_lock<std::mutex> lock(imgui_display_data.mtx_buffer);
    // set flag and wait
    imgui_display_data.stop = true;
    assert(::imgui_thread != nullptr);
    ::imgui_thread->join();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#







