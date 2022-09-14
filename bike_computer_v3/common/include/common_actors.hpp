#ifndef COMMON_ACTORS_HPP
#define COMMON_ACTORS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "actors.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
enum Sig_Ids
{
    SIG_NOT_EXIST,
    SIG_PAUSE,
    SIG_START,
    SIG_STOP,
    SIG_CONTINUE,
    SIG_NO_MAX
};

class Core0 : public Actor
{
private:
    // definde in core0.cpp
    static void handle_sig_pause(const Signal &sig);
    static void handle_sig_start(const Signal &sig);
    static void handle_sig_stop(const Signal &sig);
    static void handle_sig_continue(const Signal &sig);

    void handler_setup()
    {
        this->handler_add(handle_sig_pause, SIG_PAUSE);
        this->handler_add(handle_sig_start, SIG_START);
        this->handler_add(handle_sig_stop, SIG_STOP);
        this->handler_add(handle_sig_continue, SIG_CONTINUE);
    }
public:
    Core0()
    {
        handler_setup();
    }
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

extern Core0 actor_core0;


// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
