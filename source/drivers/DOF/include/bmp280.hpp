#ifndef BMP_280_PICO_HPP
#define BMP_280_PICO_HPP
#include <tuple>
#include <stdint.h>

namespace bmp280{
    void init();

    void update();

    std::tuple<float ,float> get_temp_press(bool update_data=false);
    /**
     * @brief calculates height above the sea with msl pressure, current pressure and temp
     *
     */
    float get_height(float pressure_msl, float pressure_cl, float temp);
};

int bmp_test();


#endif
