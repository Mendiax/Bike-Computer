#ifndef __DISPLAY_DISPLAY_FUNCTIONS_MOCK_HPP__
#define __DISPLAY_DISPLAY_FUNCTIONS_MOCK_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

/**
 * @brief namespace for running display outside of main program
 *
 */
namespace Imgui_Display
{
    /**
     * @brief starts rendering display
     *
     */
    void start(void);
    /**
     * @brief stops rendering display
     *
     */
    void stop(void);
};

#endif
