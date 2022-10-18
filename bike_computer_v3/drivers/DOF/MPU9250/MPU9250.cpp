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
#define WIA_VAL 0b01001000

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

static Vector3<int16_t> get_gyro_raw();

static Vector3<int16_t> get_gyro_raw()
{
  Vector3<int16_t> gyro_raw;
  I2C_ReadBuff(ACCEL_ADDRESS, GYRO_XOUT_H, 6, ::buf);
  gyro_raw.x = (buf[0]<<8)|buf[1];
  gyro_raw.y = (buf[2]<<8)|buf[3];
  gyro_raw.z = (buf[4]<<8)|buf[5];

  TRACE_DEBUG(2, TRACE_MPU9250, "read gyro " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(gyro_raw.arr));

  return gyro_raw;
}

// first read
static Vector3<float> gyro_zero;
/**
  * @brief  Initializes MPU9250
  * @param  None
  * @retval None
  */
	void mpu9250::init(void)
{
  TRACE_DEBUG(1, TRACE_MPU9250, "MPU9250_Init()\n");


  // config
  TRACE_DEBUG(1, TRACE_MPU9250, "Config [start]\n");
  I2C_WriteOneByte(GYRO_ADDRESS,PWR_MGMT_1, 0x00);
	I2C_WriteOneByte(GYRO_ADDRESS,SMPLRT_DIV, 0x07);
	I2C_WriteOneByte(GYRO_ADDRESS,CONFIG, 0x06);
	I2C_WriteOneByte(GYRO_ADDRESS,GYRO_CONFIG, 0x10);
	I2C_WriteOneByte(GYRO_ADDRESS,ACCEL_CONFIG, 0x01);
  TRACE_DEBUG(1, TRACE_MPU9250, "Config [done]\n");



    // // enable bypass
    // I2C_WriteOneByte(GYRO_ADDRESS, 0x37, 0x02);
    // sleep_ms(1000);

    // //read AK8963 WHOAMI

    // uint8_t readData = I2C_ReadOneByte(MAG_ADDRESS, 0x0);
    // printf("MAG WHO AM I is (Must return 72): %d\r\n", readData == 72);


     // First extract the factory calibration for each magnetometer axis
  //  uint8_t rawData[3];  // x/y/z gyro calibration data stored here
  //  uint8_t Mmode = 0b0010;
  //  float magCalibration[3];

  // #define AK8963_ADDRESS   0x0C
  // #define WHO_AM_I_AK8963  0x00 // should return 0x48
  // #define INFO             0x01
  // #define AK8963_ST1       0x02  // data ready status bit 0
  // #define AK8963_XOUT_L    0x03  // data
  // #define AK8963_XOUT_H    0x04
  // #define AK8963_YOUT_L    0x05
  // #define AK8963_YOUT_H    0x06
  // #define AK8963_ZOUT_L    0x07
  // #define AK8963_ZOUT_H    0x08
  // #define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
  // #define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
  // #define AK8963_CNTL2     0x0B  // Reset
  // #define AK8963_ASTC      0x0C  // Self test control
  // #define AK8963_I2CDIS    0x0F  // I2C disable
  // #define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
  // #define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
  // #define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value
  // #define I2C_SLV0_ADDR    0x25
  // #define I2C_SLV0_REG     0x26
  // #define I2C_SLV0_CTRL    0x27
  // #define I2C_SLV0_DO      0x63

  // #define EXT_SENS_DATA_00 0x49

  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_ADDR, AK8963_ADDRESS);           // Set the I2C slave address of AK8963 and set for write.
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_REG, AK8963_CNTL2);              // I2C slave 0 register address from where to begin data transfer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_DO, 0x01);                       // Reset AK8963
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_CTRL, 0x81);                     // Enable I2C and write 1 byte
  //  sleep_ms(50);
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_ADDR, AK8963_ADDRESS);           // Set the I2C slave address of AK8963 and set for write.
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_REG, AK8963_CNTL);               // I2C slave 0 register address from where to begin data transfer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_DO, 0x00);                       // Power down magnetometer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_CTRL, 0x81);                     // Enable I2C and write 1 byte
  //  sleep_ms(50);
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_ADDR, AK8963_ADDRESS);           // Set the I2C slave address of AK8963 and set for write.
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_REG, AK8963_CNTL);               // I2C slave 0 register address from where to begin data transfer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_DO, 0x0F);                       // Enter fuze mode
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_CTRL, 0x81);                     // Enable I2C and write 1 byte
  //  sleep_ms(50);

  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);    // Set the I2C slave address of AK8963 and set for read.
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_REG, AK8963_ASAX);               // I2C slave 0 register address from where to begin data transfer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_CTRL, 0x83);                     // Enable I2C and read 3 bytes
  //  sleep_ms(50);
  //  I2C_ReadBuff(DEFAULT_ADDRESS, EXT_SENS_DATA_00, 3, &rawData[0]);        // Read the x-, y-, and z-axis calibration values
  //  magCalibration[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;        // Return x-axis sensitivity adjustment values, etc.
  //  magCalibration[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
  //  magCalibration[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;

  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_ADDR, AK8963_ADDRESS);           // Set the I2C slave address of AK8963 and set for write.
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_REG, AK8963_CNTL);               // I2C slave 0 register address from where to begin data transfer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_DO, 0x00);                       // Power down magnetometer
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_CTRL, 0x81);                     // Enable I2C and transfer 1 byte
  //  sleep_ms(50);

  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_ADDR, AK8963_ADDRESS);           // Set the I2C slave address of AK8963 and set for write.
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_REG, AK8963_CNTL);               // I2C slave 0 register address from where to begin data transfer
  //  // Configure the magnetometer for continuous read and highest resolution
  //  // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
  //  // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_DO, 0b1000 << 4 | Mmode);        // Set magnetometer data resolution and sample ODR
  //  I2C_WriteOneByte(DEFAULT_ADDRESS, I2C_SLV0_CTRL, 0x81);                     // Enable I2C and transfer 1 byte
  //  sleep_ms(50);


  //   uint8_t readData = I2C_ReadOneByte(MAG_ADDRESS, 0x0);
  //   printf("MAG WHO AM I is (Must return 72): %d\r\n", readData == 72);


    // /*
    //     disable the I2C Master I/F module; pins ES_DA and ES_SCL are isolated
    //     from pins SDA/SDI and SCL/ SCLK.
    // */
    // I2C_WriteOneByte(GYRO_ADDRESS,USER_CTRL_AD, 0x00);
    // /*
    //     When asserted, the i2c_master interface pins(ES_CL and ES_DA) will go
    //     into bypass mode when the i2c master interface is disabled.
    // */
    // I2C_WriteOneByte(GYRO_ADDRESS,INT_BYPASS_CONFIG_AD, 0x02);

    // // setup the Magnetometer to fuse ROM access mode to get the Sensitivity
    // // Adjustment values and 16-bit output
    // I2C_WriteOneByte(MAG_AD,CNTL1_AD, 0x1F);

    // //wait for the mode changes
    // sleep_ms(100);

    // //read the Sensitivit Adjustment values
    // uint8_t buf_asax_ad[3];
    // I2C_ReadBuff(MAG_AD, ASAX_AD, 3, buf_asax_ad);
    // float asax = (buf_asax_ad[0]-128)*0.5/128+1;
    // float asay = (buf_asax_ad[1]-128)*0.5/128+1;
    // float asaz = (buf_asax_ad[2]-128)*0.5/128+1;

    // //reset the Magnetometer to power down mode
    // I2C_WriteOneByte(MAG_AD,CNTL1_AD, 0x00);

    // //wait for the mode changes
    // sleep_ms(100);

    // //set the Magnetometer to continuous mode 2(100Hz) and 16-bit output
    // I2C_WriteOneByte(MAG_AD,CNTL1_AD, 0x16);

    // //wait for the mode changes
    // sleep_ms(100);

	sleep_ms(10);
  PRINTF("mpu9250::check()\n");
	if(!mpu9250::check())
	{
    TRACE_ABNORMAL(TRACE_MPU9250,"MPU9250_Check() false %" PRIu8 "\n", I2C_ReadOneByte(DEFAULT_ADDRESS, WHO_AM_I));
	}
  enum {NO_READ=100};
  get_gyro_raw();
  gyro_zero= {0.0f,0.0f,0.0f};
  for(int i = 0; i < NO_READ; i++)
  {
    sleep_ms(100);
    gyro_zero = gyro_zero + (Vector3<float>)get_gyro_raw();
  }
  gyro_zero = (gyro_zero) / (float)NO_READ;
  TRACE_DEBUG(2, TRACE_MPU9250, "read gyro offsets = " FORMAT_FLOAT_ARR " \n", INS_ARR(gyro_zero.arr));

  // gyro_zero = get_gyro_raw();
}
/**
  * @brief Get accelerometer datas
  * @param  None
  * @retval None
  */
void mpu9250::read_accel(Vector3<int16_t>& accel_data)
{
  //consolep("MPU9250_READ_ACCEL()\n");
  I2C_ReadBuff(ACCEL_ADDRESS, ACCEL_XOUT_H, 6, ::buf);
  accel_data.x = (buf[0]<<8)|buf[1];
  accel_data.y = (buf[2]<<8)|buf[3];
  accel_data.z = (buf[4]<<8)|buf[5];
  TRACE_DEBUG(4, TRACE_MPU9250, "read accel " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(accel_data.arr));
}

/**
  * @brief Get gyroscopes datas
  * @param  None
  * @retval None
  */
void mpu9250::read_gyro(Vector3<float>& gyro_data)
{
  static Vector3<float> gyro_angle{0.0f,0.0f,0.0f};
  static absolute_time_t gyro_update;
  static bool first_read = true;

  //uptade time
  auto current = get_absolute_time();
  const double delta_s = (double)us_to_ms(absolute_time_diff_us(gyro_update, current));
  gyro_update = current;


  Vector3 gyro_raw = get_gyro_raw();
  // if(first_read)
  // {
  //   first_read = false;
  //   return;
  // }
  const float sensitivity = 1.0323;
  Vector3<float> gyro_rate =((Vector3<float>)gyro_raw - gyro_zero) / sensitivity;
  TRACE_DEBUG(2, TRACE_MPU9250, "gyro rate " FORMAT_FLOAT_ARR " \n", INS_ARR(gyro_rate.arr));


  gyro_angle = gyro_angle + (gyro_rate * (delta_s / 1000.0));
  gyro_data = gyro_angle;


}
/**
  * @brief Get compass datas
  * @param  None
  * @retval None
  */
void mpu9250::read_mag(Vector3<int16_t>& mag_data)
{
  // ?????????????????
  I2C_WriteOneByte(GYRO_ADDRESS,0x37,0x02);//turn on Bypass Mode
  sleep_ms(10);
  I2C_WriteOneByte(MAG_ADDRESS,0x0A,0x01);
  sleep_ms(10);
  static Vector3<> mag;


  uint8_t wia_resp = I2C_ReadOneByte(MAG_ADDRESS, WIA_AD);

  if(wia_resp != WIA_VAL)
  {
    consolep("MAGNETOMETR FAILED\n");
  }


  uint8_t status;
  I2C_ReadBuff(MAG_ADDRESS, STATUS_1_AD, 1, &status);
  if((status & 0x01) == 0x01){
    I2C_ReadBuff(MAG_ADDRESS, MAG_XOUT_L, 6, ::buf);
     if(!(buf[6] & 0x8)){
      // little endian so we flip order of bytes
      mag.x = int16_t(buf[1]<<8) | int16_t(buf[0]);
      mag.y = int16_t(buf[3]<<8) | int16_t(buf[2]);
      mag.z = int16_t(buf[5]<<8) | int16_t(buf[4]);
     }
  }


  mag_data = mag;
  TRACE_DEBUG(3, TRACE_MPU9250, "read mag " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(mag_data.arr));
}

/**
  * @brief  Check MPU9250,ensure communication succeed
  * @param  None
  * @retval true: communicate succeed
  *               false: communicate fualt
  */
bool mpu9250::check(void)
{
  //consolep("MPU9250_Check()\n");
  return WHO_AM_I_VAL == I2C_ReadOneByte(DEFAULT_ADDRESS, WHO_AM_I);
}
