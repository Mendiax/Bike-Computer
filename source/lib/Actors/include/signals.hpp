#ifndef SIGNALS_HPP
#define SIGNALS_HPP

// #-------------------------------#
// |           includes            |
// #-------------------------------#
// Pico includes

// C/C++ includes
#include <cstdint>
#include <typeinfo>
#include <cstring>
#include <cstdio>

// My includes

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
        : id{id}, payload_size{0}
    {
        this->payload = nullptr;
    }

    template<typename T>
    Signal(sig_id id, T* payload)
        : id{id}, payload_size{sizeof(T)}
    {
        this->payload = reinterpret_cast<char *>(payload);
    }

    sig_id get_sig_id() const
    {
        return this->id;
    }

    template <typename T>
    inline T get_payload() const
    {
        return *reinterpret_cast<T*>(this->payload);
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

// No global variables declarations

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif // SIGNALS_HPP
