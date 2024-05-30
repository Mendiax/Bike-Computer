#ifndef __MPU9250_H__
#define __MPU9250_H__

#include "MPU9250.hpp"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <tuple>

#include <vector3.hpp>

void mpu_interrupt_callback();

namespace mpu9250 {
/**
 * @brief Initialize MPU
 *
 * @param interrupt
 */
void init(bool interrupt = false);
/**
 * @brief  Check MPU9250,ensure communication succeed
 * @param  None
 * @retval true: communicate succeed
 *               false: communicate fualt
 */
bool check(void);
void set_reference(Vector3<float> &gyro_angle_data,
                   Vector3<float> &gyro_rotation_data,
                   Vector3<int16_t> &accel_data, Vector3<int16_t> &mag_data);

void get_rotation(Vector3<float> &gyro_data, Vector3<float> &rotation_data);
/**
 * @brief Get gyroscopes datas
 * @param  None
 * @retval None
 */
void read_gyro(Vector3<int16_t> &gyro_data);
/**
 * @brief Get accelerometer datas
 * @param  None
 * @retval None
 */
void read_accel(Vector3<int16_t> &accel_data);
/**
 * @brief Get compass datas
 * @param  None
 * @retval None
 */
void read_mag(Vector3<int16_t> &mag_data);

void update();

void get_mpu_data(Vector3<float> &gyro_angle_data,
                  Vector3<float> &gyro_rotation_data,
                  Vector3<int16_t> &accel_data, Vector3<int16_t> &mag_data);
} // namespace mpu9250

#endif