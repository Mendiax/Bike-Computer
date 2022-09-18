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
    // core 0
    SIG_CORE0_NOT_EXIST,
    SIG_CORE0_SESION_START,
    SIG_CORE0_PAUSE,
    SIG_CORE0_START,
    SIG_CORE0_STOP,
    SIG_CORE0_CONTINUE,
    // core 1
    SIG_NO_MAX
};

struct Sig_Core1_Total_Update{
    float ridden_dist;
    float ridden_time;
};

class Core0 : public Actor
{
private:
    // definde in core0.cpp

    static void handle_sig_session_start(const Signal &sig);
    static void handle_sig_pause(const Signal &sig);
    static void handle_sig_start(const Signal &sig);
    static void handle_sig_stop(const Signal &sig);
    static void handle_sig_continue(const Signal &sig);

    void handler_setup()
    {
        this->handler_add(handle_sig_pause, SIG_CORE0_PAUSE);
        this->handler_add(handle_sig_continue, SIG_CORE0_CONTINUE);
        this->handler_add(handle_sig_start, SIG_CORE0_START);
        this->handler_add(handle_sig_stop, SIG_CORE0_STOP);
        this->handler_add(handle_sig_session_start, SIG_CORE0_SESION_START);
    }
public:
    Core0()
    {
        handler_setup();
    }
};

class Core1 : public Actor
{
private:
    // definde in core1.cpp

    void handler_setup()
    {

    }
public:
    Core1()
    {
        handler_setup();
    }
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

extern Core0 actor_core0;
extern Core1 actor_core1;



// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
