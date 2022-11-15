#ifndef SIGNALS_HPP
#define SIGNALS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
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
    char *payload;
    void* actor_p;
    const size_t payload_size;

public:
    Signal(sig_id id)
        : id{id}, actor_p{nullptr}, payload_size{0}
    {
        this->payload = nullptr;
    }
    template<typename T>
    Signal(sig_id id, T* payload)
        : id{id}, actor_p{nullptr}, payload_size{sizeof(T)}
    {
        this->payload = (char *)payload;
        // this->payload = new char[payload_size];
        // memcpy(this->payload, &payload, payload_size);
        // printf("payload = %p\n", this->payload);
        // printf("payload cont = %p\n", payload);

        // T payload_cop;
        // memcpy(&payload_cop, this->payload, this->payload_size);
        // printf("payload copied = %p\n", payload_cop);
    }
    // ~Signal()
    // {
    //     //delete[] this->payload;
    // }
    sig_id get_sig_id() const
    {
        return this->id;
    }
    template <typename T>
    inline T get_payload(void) const
    {
        //T payload;
        //memcpy(&payload, this->payload, this->payload_size);
        //printf("payload copied = %p\n", payload);

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

// template<typename T>
// struct Signal
// {
//     const uint16_t sig_id;
//     const T payload;

//     Signal(const T &payload);
//     T get_signal() const;
// };

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
