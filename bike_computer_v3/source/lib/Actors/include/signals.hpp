#ifndef SIGNALS_HPP
#define SIGNALS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <cstdint>
#include <typeinfo>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>


// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

typedef uint16_t sig_id;

struct Signal
{
private:
    const sig_id id;
    const size_t payload_size;
    char *payload;
    void* actor_p;

public:
    Signal(sig_id id)
        : id{id}, payload_size{0}, actor_p{0}
    {
        this->payload = nullptr;
    }

    template<typename T>
    Signal(sig_id id, T* payload)
        : id{id}, payload_size{sizeof(T)}, actor_p{0}
    {
        this->payload = (char *)payload;
    }

    template<typename T>
    Signal(sig_id id, T* payload, void* actor_p)
        : id{id}, payload_size{sizeof(T)}, actor_p{actor_p}
    {
        this->payload = (char *)payload;
    }

    sig_id get_sig_id() const
    {
        return this->id;
    }
    template <typename T>
    inline T get_payload(void) const
    {
        return (T)this->payload;
    }
    inline void set_actor(void* actor)
    {
        actor_p = actor;
    }
    inline void* get_actor() const
    {
        return actor_p;
    }
};


// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
