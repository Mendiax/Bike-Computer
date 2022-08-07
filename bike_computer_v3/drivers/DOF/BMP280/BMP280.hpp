#ifndef BMP_280_PICO_HPP
#define BMP_280_PICO_HPP
#include <tuple>
#include <stdint.h>

namespace bmp280{
    void init();
    std::tuple<int32_t ,int32_t> get_temp_press();
};

int bmp_test();






#endif
