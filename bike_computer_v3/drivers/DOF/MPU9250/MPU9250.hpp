/**
  ******************************************************************************
  * @file    MPU9250.h
  * @author  
  * @version V1.0
  * @date    27-January-2015
  * @brief   Header file for MPU9250.c module.
  
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WAVESHARE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 Waveshare</center></h2>
  ******************************************************************************
  */


#ifndef __MPU9250_H
#define __MPU9250_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
//#include "I2C.h"
//#include "IMU.H"



// define MPU9250 register address
//****************************************
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


struct Vector3
{
  union{
    struct
    {
      int16_t x;
      int16_t y;
      int16_t z;
    };
    int16_t arr[3];
  };

  void normalize()
  {
    const int scale = 100;
    auto vector_length = std::sqrt(x*x + y*y + z*z);
    x = (100 * x) / vector_length;
    y = (100 * y) / vector_length;
    z = (100 * z) / vector_length;
  }
};

namespace mpu9250
{
  void init(void);
  void read_accel(Vector3& accel_data);
  void read_gyro(Vector3& gyro_data);
  void read_mag(Vector3& mag_data);
  bool check(void);
}




#endif
