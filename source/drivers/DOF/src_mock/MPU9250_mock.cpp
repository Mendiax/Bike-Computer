// #include <math.h>
#include "pico/stdlib.h"


#include "MPU9250.hpp"
#include <interrupts/interrupts.hpp>
#include "traces.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#


// #------------------------------#
// | global variables definitions |
// #------------------------------#


// #------------------------------#
// | static variables definitions |
// #------------------------------#


// Interrupt data
static int callback_count = 0;

// contains current angle data
static Vector3<float>* gyro_angle;
static Vector3<float>* gyro_rotation;
static Vector3<int16_t>* gyro_accel;
static Vector3<int16_t>* mag_vector;




// #------------------------------#
// | static functions declarations|
// #------------------------------#
// calibration data
static Vector3<float> gyro_zero;

static void mpu9250_setup_i2c_config();
static void mpu9250_calibrate_gyro();


void mpu9250::init(bool interrupt)
{
  gyro_angle = new Vector3<float>{0.0f,0.0f,0.0f};
  gyro_rotation = new Vector3<float>{0.0f,0.0f,0.0f};
  gyro_accel = new Vector3<int16_t>{(int16_t)0,(int16_t)0,(int16_t)0};
  mag_vector = new Vector3<int16_t>{(int16_t)0,(int16_t)0,(int16_t)0};


  TRACE_DEBUG(2, TRACE_MPU9250, "read gyro offsets = " FORMAT_FLOAT_ARR " \n", INS_ARR(gyro_zero.arr));
  if(interrupt)
  {
    Interrupt interrupt_mpu = {20, mpu9250::update, GPIO_IRQ_EDGE_RISE};
    interrupt_add(interrupt_mpu, CORE_0, false);
  }
}

bool mpu9250::check(void)
{
  return true;;
}

void mpu9250::set_reference(Vector3<float> &gyro_angle_data,
                       Vector3<float> &gyro_rotation_data,
                       Vector3<int16_t> &accel_data,
                       Vector3<int16_t> &mag_data) {
  delete gyro_angle;
  delete gyro_rotation;
  delete gyro_accel;
  delete mag_vector;

  gyro_accel = &accel_data;
  gyro_angle = &gyro_angle_data;
  gyro_rotation = &gyro_rotation_data;
  mag_vector = &mag_data;
}






void mpu9250::get_rotation(Vector3<float>& gyro_data, Vector3<float>& rotation_data)
{
  static absolute_time_t gyro_update;
  static Vector3 gyro_raw;

  //update time
  auto current = get_absolute_time();
  const double delta_s = (double)us_to_ms(absolute_time_diff_us(gyro_update, current));
  gyro_update = current;


  // read data
  mpu9250::read_gyro(gyro_raw);

  // calculate rotation speed
  const float sensitivity = 1.0323;
  rotation_data =((Vector3<float>)gyro_raw - gyro_zero) / sensitivity;

  // calculation rotation
  gyro_data = gyro_data + (rotation_data * (delta_s / 1000.0));
}

void mpu9250::read_gyro(Vector3<int16_t>& gyro_data)
{
  gyro_data.x = 0;
  gyro_data.y = 0;
  gyro_data.z = 0;
}

void mpu9250::read_accel(Vector3<int16_t>& accel_data)
{
  //consolep("MPU9250_READ_ACCEL()\n");
  accel_data.x = 0;
  accel_data.y = 0;
  accel_data.z = 0;
}

void mpu9250::read_mag(Vector3<int16_t>& mag_data)
{
  mag_data.x = 0;
  mag_data.y = 0;
  mag_data.z = 0;
}


void mpu9250::update()
{
  callback_count++;
  mpu9250::get_rotation(*gyro_angle, *gyro_rotation);
  mpu9250::read_accel(*gyro_accel);
  mpu9250::read_mag(*mag_vector);
}




static void mpu9250_setup_i2c_config(){
  TRACE_DEBUG(1, TRACE_MPU9250, "Config [start]\n");
  TRACE_DEBUG(1, TRACE_MPU9250, "Config [done]\n");
}

static void mpu9250_calibrate_gyro() {
  enum {NO_READ=100};
  Vector3<int16_t> gyro_read_vec;
  // clear buffer
  mpu9250::read_gyro(gyro_read_vec);
  // reset vector
  gyro_zero= {0.0f,0.0f,0.0f};
  for(int i = 0; i < NO_READ; i++)
  {
    mpu9250::read_gyro(gyro_read_vec);
    sleep_ms(100);
    gyro_zero = gyro_zero + (Vector3<float>)gyro_read_vec;
  }
  gyro_zero = (gyro_zero) / (float)NO_READ;
}


