/**
  ******************************************************************************
  * @file    MPU9250.c
  * @author  
  * @version V1.0
  * @date    27-January-2015
  * @brief   This file includes the MPU9250 driver functions
  
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


#include "MPU9250.hpp"
#include "I2C.h"
#include <math.h>
#include "console/console.h"
#include "pico/stdlib.h"
#include "traces.h"
#include "inttypes.h"


static uint8_t buf[6]; // common buffer for all reads
/**
  * @brief  Initializes MPU9250
  * @param  None
  * @retval None
  */
	void mpu9250::init(void)
{
  TRACE_DEBUG(1, TRACE_MPU9250, "MPU9250_Init()\n");


  // config
  I2C_WriteOneByte(GYRO_ADDRESS,PWR_MGMT_1, 0x00);
	I2C_WriteOneByte(GYRO_ADDRESS,SMPLRT_DIV, 0x07);
	I2C_WriteOneByte(GYRO_ADDRESS,CONFIG, 0x06);
	I2C_WriteOneByte(GYRO_ADDRESS,GYRO_CONFIG, 0x10);
	I2C_WriteOneByte(GYRO_ADDRESS,ACCEL_CONFIG, 0x01);
	
	sleep_ms(10);
	if(!mpu9250::check())
	{
    TRACE_ABNORMAL(TRACE_MPU9250,"MPU9250_Check() false\n%s", "");
	}
}
/**
  * @brief Get accelerometer datas
  * @param  None
  * @retval None
  */
void mpu9250::read_accel(Vector3& accel_data)
{ 
  //consolep("MPU9250_READ_ACCEL()\n");
  I2C_ReadBuff(ACCEL_ADDRESS, ACCEL_XOUT_H, 6, ::buf);
  accel_data.x = (buf[0]<<8)|buf[1];
  accel_data.y = (buf[2]<<8)|buf[3];
  accel_data.z = (buf[4]<<8)|buf[5];
  TRACE_DEBUG(2, TRACE_MPU9250, "read accel " FORMAT_CREATE_TEMPLATE("%" PRIu16) " \n", INS_ARR(accel_data.arr));
}
/**
  * @brief Get gyroscopes datas
  * @param  None
  * @retval None
  */
void mpu9250::read_gyro(Vector3& gyro_data)
{
  static Vector3 pos;
  I2C_ReadBuff(ACCEL_ADDRESS, GYRO_XOUT_H, 6, ::buf);
  pos.x += (buf[0]<<8)|buf[1];
  pos.y += (buf[2]<<8)|buf[3];
  pos.z += (buf[4]<<8)|buf[5];

  gyro_data = pos;

  TRACE_DEBUG(2, TRACE_MPU9250, "read gyro " FORMAT_CREATE_TEMPLATE("%" PRIu16) " \n", INS_ARR(gyro_data.arr));
}
/**
  * @brief Get compass datas
  * @param  None
  * @retval None
  */
void mpu9250::read_mag(Vector3& mag_data)
{
  // ?????????????????
  I2C_WriteOneByte(GYRO_ADDRESS,0x37,0x02);//turn on Bypass Mode 
  sleep_ms(10);
  I2C_WriteOneByte(MAG_ADDRESS,0x0A,0x01);	
  sleep_ms(10);
  I2C_ReadBuff(ACCEL_ADDRESS, MAG_XOUT_L, 6, ::buf);
  // little endian so we flip order of bytes
  mag_data.x = (buf[1]<<8)|buf[0];
  mag_data.y = (buf[3]<<8)|buf[2];
  mag_data.z = (buf[5]<<8)|buf[4];

  TRACE_DEBUG(2, TRACE_MPU9250, "read mag " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(mag_data.arr));
}

/**
  * @brief  Check MPU9250,ensure communication succeed
  * @param  None
  * @retval true: communicate succeed
  *               false: communicate fualt 
  */
bool mpu9250::check(void) 
{
  consolep("MPU9250_Check()\n");
  return WHO_AM_I_VAL == I2C_ReadOneByte(DEFAULT_ADDRESS, WHO_AM_I);
}
