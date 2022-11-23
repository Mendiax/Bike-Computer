#ifndef COMMON_ACTORS_HPP
#define COMMON_ACTORS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <cstdint>
#include <string>

// my includes
#include "common_types.h"
#include "actors.hpp"
#include "display/driver.hpp"
#include "pico/stdlib.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

namespace actors_common {
    enum Sig_Ids
    {
        // core 0
        SIG_DATA_ACTOR_NOT_EXIST,
        SIG_DATA_ACTOR_SESSION_START,
        SIG_DATA_ACTOR_SESSION_LOAD,
        SIG_DATA_ACTOR_PAUSE,
        SIG_DATA_ACTOR_START,
        SIG_DATA_ACTOR_STOP,
        SIG_DATA_ACTOR_CONTINUE,
        SIG_DATA_ACTOR_SET_CONFIG,
        SIG_DATA_ACTOR_SET_TOTAL,
        SIG_DATA_ACTOR_GET_FILE_RESPOND,

        // core 1
        SIG_DISPLAY_ACTOR_TOTAL_UPDATE,
        SIG_DISPLAY_ACTOR_START_PAUSE_BTN,
        SIG_DISPLAY_ACTOR_END_BTN,
        SIG_DISPLAY_ACTOR_LOAD_SESSION,
        SIG_DISPLAY_ACTOR_LOG_GPS,
        SIG_DISPLAY_ACTOR_LOG,
        SIG_DISPLAY_ACTOR_GET_FILE,
        SIG_DISPLAY_ACTOR_SHOW_MSG,
        SIG_NO_MAX
    };
    enum class File_Respond { time_offset };

}

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#


// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
