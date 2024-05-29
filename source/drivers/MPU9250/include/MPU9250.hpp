#ifndef __MPU9250_H__
#define __MPU9250_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <tuple>




// define MPU9250 register address
#define	SMPLRT_DIV		0x19	//Sample Rate Divider. Typical values:0x07(125Hz) 1KHz internal sample rate
#define	CONFIG				0x1A	//Low Pass Filter.Typical values:0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//Gyro Full Scale Select. Typical values:0x10(1000dps)
#define	ACCEL_CONFIG	0x1C	//Accel Full Scale Select. Typical values:0x01(2g)

#define	ACCEL_XOUT_H	0x3B
// #define	ACCEL_XOUT_L	0x3C
// #define	ACCEL_YOUT_H	0x3D
// #define	ACCEL_YOUT_L	0x3E
// #define	ACCEL_ZOUT_H	0x3F
// #define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
// #define	GYRO_XOUT_L		0x44
// #define	GYRO_YOUT_H		0x45
// #define	GYRO_YOUT_L		0x46
// #define	GYRO_ZOUT_H		0x47
// #define	GYRO_ZOUT_L		0x48


#define MAG_XOUT_L		0x03
// #define MAG_XOUT_H		0x04
// #define MAG_YOUT_L		0x05
// #define MAG_YOUT_H		0x06
// #define MAG_ZOUT_L		0x07
// #define MAG_ZOUT_H		0x08


#define	PWR_MGMT_1		0x6B	//Power Management. Typical values:0x00(run mode)
#define	WHO_AM_I		  0x75	//identity of the device


#define DEFAULT_ADDRESS     0x68

#define	GYRO_ADDRESS   DEFAULT_ADDRESS //Gyro and Accel device address
#define ACCEL_ADDRESS  DEFAULT_ADDRESS //0xD0
#define MAG_ADDRESS    0xC //0x18   //compass device address

#define WHO_AM_I_VAL				0x71 //identity of MPU9250 is 0x71. identity of MPU9255 is 0x73.


void mpu_interrupt_callback();


template<typename T = int16_t>
struct Vector3
{
  union{
    struct
    {
      T x;
      T y;
      T z;
    };
    T arr[3];
  };

  void normalize()
  {
    const int scale = 100;
    auto vector_length = std::sqrt(x*x + y*y + z*z);
    x = (100.0f * (float)x) / vector_length;
    y = (100.0f * (float)y) / vector_length;
    z = (100.0f * (float)z) / vector_length;
  }
  Vector3<T> operator- (const Vector3<T>& other)
  {
    T x = this->x - other.x;
    T y = this->y - other.y;
    T z = this->z - other.z;
    return Vector3<T>{x,y,z};
  }
  Vector3<T> operator+ (const Vector3<T>& other)
  {
    T x = this->x + other.x;
    T y = this->y + other.y;
    T z = this->z + other.z;
    return Vector3<T>{x,y,z};
  }
  template<typename Q>
  Vector3<T> operator* (const Q constant)
  {
    T x = this->x * constant;
    T y = this->y * constant;
    T z = this->z * constant;
    return Vector3<T>{x,y,z};
  }
  template<typename Q>
  Vector3<T> operator/ (const Q constant)
  {
    T x = this->x / constant;
    T y = this->y / constant;
    T z = this->z / constant;
    return Vector3<T>{x,y,z};
  }
  template<typename Q>
  explicit operator Vector3<Q>() {
    return Vector3<Q>{(Q)x, (Q)y, (Q)z};
  }
};

namespace mpu9250
{
  void init(bool interrupt=false);
  void read_accel(Vector3<int16_t>& accel_data);
  void read_gyro(Vector3<float>& gyro_data);
  void read_mag(Vector3<int16_t>& mag_data);
  bool check(void);
  std::tuple<Vector3<float>, Vector3<int16_t>> get_mpu_data();

}




#endif