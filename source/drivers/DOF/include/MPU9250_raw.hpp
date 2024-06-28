#ifndef __MPU9250_RAW_H__
#define __MPU9250_RAW_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <tuple>
#include "vector3.hpp"







void mpu_interrupt_callback();

void mpu9250_init();
void mpu9250_config();
Vector3<int16_t> mpu9250_calibrate();

#endif