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

//MPU9250
#define MPU9250_AD 0x68
#define FIFO_EN_AD 0x23
#define PWR_MGMT_1_AD 0x6B
#define ACCEL_XOUT_H_AD 0x3B
#define GYRO_XOUT_H_AD 0x43
#define EXT_SENS_DATA_00_AD 0x49
#define ACCEL_CONFIG_1_AD 0x1C
#define ACCEL_CONFIG_2_AD 0x1D
#define GYRO_CONFIG_AD 0x1B
#define CONFIG_AD 0x1A
#define I2C_MST_CTRL_AD 0x24
#define I2C_SLV0_ADDR_AD 0x25
#define I2C_SLV0_REG_AD 0x26
#define I2C_SLV0_CTRL_AD 0x27
#define INT_BYPASS_CONFIG_AD 0x37
#define USER_CTRL_AD 0x6A
#define ACCEL_SENS 8192.0f
#define GYRO_SENS 32.8f

//Magnetometer
#define MAG_AD 0xC
#define WIA_AD 0x00
#define INFO 0x01
#define STATUS_1_AD 0x02
#define HXL_AD 0x03    //X-axis measurement data lower 8bit
#define HXH_AD 0x04    //X-axis measurement data higher 8bit
#define HYL_AD 0x05    //Y-axis measurement data lower 8bit
#define HYH_AD 0x06    //Y-axis measurement data higher 8bit
#define HZL_AD 0x07    //Z-axis measurement data lower 8bit
#define HZH_AD 0x08    //Z-axis measurement data higher 8bit
#define STATUS_2_AD 0x09
#define CNTL1_AD 0x0A   //control 1
#define CNTL2_AD 0x0B   //control 2
#define ASTC_AD 0x0C    //Self-Test Control
#define TS1_AD 0x0D    //test 1
#define TS2_AD 0x0E   //test 2
#define I2CDIS_AD 0x0F    //I2C disable
#define ASAX_AD 0x10    //Magnetic sensor X-axis sensitivity adjustment value
#define ASAY_AD 0x11    //Magnetic sensor Y-axis sensitivity adjustment value
#define ASAZ_AD 0x12    //Magnetic sensor Z-axis sensitivity adjustment value
#define MAGNE_SENS 6.67f
#define SCALE 0.1499f  // 4912/32760 uT/tick


static uint8_t buf[7]; // common buffer for all reads
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


	
    /*
        disable the I2C Master I/F module; pins ES_DA and ES_SCL are isolated
        from pins SDA/SDI and SCL/ SCLK.
    */
    I2C_WriteOneByte(GYRO_ADDRESS,USER_CTRL_AD, 0x00);
    /*
        When asserted, the i2c_master interface pins(ES_CL and ES_DA) will go
        into bypass mode when the i2c master interface is disabled.
    */
    I2C_WriteOneByte(GYRO_ADDRESS,INT_BYPASS_CONFIG_AD, 0x02);

    // setup the Magnetometer to fuse ROM access mode to get the Sensitivity
    // Adjustment values and 16-bit output
    I2C_WriteOneByte(MAG_AD,CNTL1_AD, 0x1F);

    //wait for the mode changes
    sleep_ms(100);

    //read the Sensitivit Adjustment values
    uint8_t buf_asax_ad[3];
    I2C_ReadBuff(MAG_AD, ASAX_AD, 3, buf_asax_ad);
    float asax = (buf_asax_ad[0]-128)*0.5/128+1;
    float asay = (buf_asax_ad[1]-128)*0.5/128+1;
    float asaz = (buf_asax_ad[2]-128)*0.5/128+1;

    //reset the Magnetometer to power down mode
    I2C_WriteOneByte(MAG_AD,CNTL1_AD, 0x00);

    //wait for the mode changes
    sleep_ms(100);

    //set the Magnetometer to continuous mode 2(100Hz) and 16-bit output
    I2C_WriteOneByte(MAG_AD,CNTL1_AD, 0x16);

    //wait for the mode changes
    sleep_ms(100);
	
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
  // I2C_WriteOneByte(MAG_ADDRESS,0x37,0x02);//turn on Bypass Mode 
  // sleep_ms(10);
  // I2C_WriteOneByte(MAG_ADDRESS,0x0A,0x01);	
  // sleep_ms(10);
  static Vector3 mag;

  uint8_t status;
  I2C_ReadBuff(MAG_ADDRESS, STATUS_1_AD, 1, &status);
  if((status & 0x01) == 0x01){
    I2C_ReadBuff(MAG_ADDRESS, MAG_XOUT_L, 7, ::buf);
    if(!(buf[6] & 0x8)){
      // little endian so we flip order of bytes
      mag.x = int16_t(buf[1]<<8) | int16_t(buf[0]);
      mag.y = int16_t(buf[3]<<8) | int16_t(buf[2]);
      mag.z = int16_t(buf[5]<<8) | int16_t(buf[4]);
    }
  }


  mag_data = mag;
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
