
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "actors.hpp"
#include "traces.h"

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

Actor::Actor()
{
    mutex_init(&mutex_queue);
}

/**
 * @brief sends signal to this actor
 * It will append signal in its queue
 *
 * @param sig
 */
void Actor::send_signal(const Signal &sig)
{
    mutex_enter_blocking(&mutex_queue);
    this->sig_queue.push(sig);
    mutex_exit(&mutex_queue);
    TRACE_DEBUG(1, TRACE_ACTOR,"sent sig id=%" PRIu16 "\n", sig.get_sig_id());
}

/**
 * @brief pop from internal queue and execute signal
 *
 */
void Actor::handle_next_signal()
{
    if (this->is_queue_empty())
    {
        return;
    }

    mutex_enter_blocking(&mutex_queue);
    const Signal sig = sig_queue.front();
    sig_queue.pop();
    mutex_exit(&mutex_queue);


    bool handled = false;
    for(auto& handler : this->handlers)
    {
        if(handler.id == sig.get_sig_id())
        {
            handler.handler_func(sig);
            handled = true;
            break;
        }
    }

    if(!handled)
    {
        TRACE_ABNORMAL(TRACE_ACTOR, "Signal not handled %u\n", (unsigned)sig.get_sig_id());
    }
}

/**
 * @brief handle all signals in queue
 *
 */
void Actor::handle_all()
{
    while (!this->is_queue_empty())
    {
        this->handle_next_signal();
    }
}

/**
 * @brief checks if there is no signals in queue
 *
 * @return true
 * @return false
 */
bool Actor::is_queue_empty()
{
    mutex_enter_blocking(&mutex_queue);
    bool is_empty = this->sig_queue.empty();
    mutex_exit(&mutex_queue);
    return is_empty;
}
// void Actor::handler_setup()
// {
//     printf("default :(\n");
// }

// #------------------------------#
// | static functions definitions |
// #------------------------------#
