#ifndef FILTERLOWPASS_HPP
#define FILTERLOWPASS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <type_traits>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

template<typename T>
class Filter_Low_Pass {
    static_assert(std::is_floating_point_v<T>);
public:
    Filter_Low_Pass(double alpha) : alpha_(alpha), prev_output_(0) {}

    T apply(T input) {
        T output = alpha_ * input + (1.0f - alpha_) * prev_output_;
        prev_output_ = output;
        return output;
    }

private:
    double alpha_;
    T prev_output_;
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
