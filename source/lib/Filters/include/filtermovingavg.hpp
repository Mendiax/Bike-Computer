#pragma once
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <type_traits>

// my includes
#include "ringarray.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

template<typename T>
class Filter_Moving_Avg {
    static_assert(std::is_floating_point_v<T>);
public:
    Filter_Moving_Avg(size_t count) : count(count) {
        data_buffer.set_length(count);
    }

    T apply(T input) {
        data_buffer.add_element(input);
        auto output = 0;
        for(const auto& elem : data_buffer.get_data()) {
            output += elem;
        }
        output /= count;
        return output;
    }

private:
    size_t count;
    RingArray<T> data_buffer;
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

