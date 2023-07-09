#ifndef __BMP280_RAW_HPP__
#define __BMP280_RAW_HPP__

#include <stdint.h>

void bmp280_init();
void bmp280_update_data();
float bmp280_get_temp();
float bmp280_get_press();

#endif

