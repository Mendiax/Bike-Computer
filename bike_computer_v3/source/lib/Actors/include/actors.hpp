#ifndef ACTORS_HPP
#define ACTORS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include <pico/mutex.h>

// c/c++ includes
#include <queue>
#include <stdio.h>

// my includes
#include "signals.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

typedef void(*sig_hander)(const Signal&);

class Actor
{
    struct Handler
    {
        sig_hander handler_func;
        sig_id id;
        Handler(sig_hander handler_func, sig_id id)
            : handler_func{handler_func}, id{id}
        {}
    };
private:
    std::queue<Signal> sig_queue;
    mutex mutex_queue;
protected:
    std::vector<Handler> handlers;
    inline void handler_add(sig_hander handler, sig_id id)
    {
        this->handlers.emplace_back(handler, id);
    }

public:
    /**
     * @brief adds sig handlers to actor
     *
     */
    virtual void handler_setup() = 0;
    // {
    //     printf("default :(\n");
    // }
public:
    Actor();
    ~Actor();

    /**
     * @brief sends signal to this actor
     * It will append signal in its queue
     *
     * @param sig
     */
    void send_signal(const Signal& sig);
    void send_signal(Signal& sig, Actor* from);


    /**
     * @brief pop from internal queue and execute signal
     *
     */
    void handle_next_signal();

    /**
     * @brief handle all signals in queue
     *
     */
    void handle_all();


    /**
     * @brief checks if there is no signals in queue
     *
     * @return true
     * @return false
     */
    bool is_queue_empty();
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
