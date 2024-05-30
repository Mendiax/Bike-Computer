// #include <math.h>
#include "pico/stdlib.h"


#include "MPU9250.hpp"
#include "i2c.h"
#include <interrupts/interrupts.hpp>
#include <tuple>
#include "traces.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

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

// Address of devices
#define DEFAULT_ADDRESS     0x68
#define	GYRO_ADDRESS   DEFAULT_ADDRESS //Gyro and Accel device address
#define ACCEL_ADDRESS  DEFAULT_ADDRESS //0xD0
#define MAG_ADDRESS    0xC //0x18   //compass device address

#define WHO_AM_I_VAL				0x71 //identity of MPU9250 is 0x71. identity of MPU9255 is 0x73.


//MPU9250 registers address
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
#define INT_ENABLE_AD 0x38

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

// #------------------------------#
// | global variables definitions |
// #------------------------------#


// #------------------------------#
// | static variables definitions |
// #------------------------------#

static uint8_t buf[7]; // common buffer for all reads

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


  I2C_Init();
  // config
  mpu9250_setup_i2c_config();

	sleep_ms(10);
  PRINTF("mpu9250::check()\n");
	if(!mpu9250::check())
	{
    TRACE_ABNORMAL(TRACE_MPU9250,"MPU9250_Check() false %" PRIu8 "\n", I2C_ReadOneByte(DEFAULT_ADDRESS, WHO_AM_I));
	}

  mpu9250_calibrate_gyro();

  TRACE_DEBUG(2, TRACE_MPU9250, "read gyro offsets = " FORMAT_FLOAT_ARR " \n", INS_ARR(gyro_zero.arr));
  if(interrupt)
  {
    Interrupt interrupt_mpu = {20, mpu9250::update, GPIO_IRQ_EDGE_RISE};
    interrupt_add(interrupt_mpu, CORE_0, false);
  }
}

bool mpu9250::check(void)
{
  //consolep("MPU9250_Check()\n");
  return WHO_AM_I_VAL == I2C_ReadOneByte(DEFAULT_ADDRESS, WHO_AM_I);
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
  // TRACE_DEBUG(2, TRACE_MPU9250, "gyro rate " FORMAT_FLOAT_ARR " \n", INS_ARR(rotation_data.arr));

  // calculation rotation
  gyro_data = gyro_data + (rotation_data * (delta_s / 1000.0));
}

void mpu9250::read_gyro(Vector3<int16_t>& gyro_data)
{
  ;
  I2C_ReadBuff(ACCEL_ADDRESS, GYRO_XOUT_H, 6, ::buf);
  gyro_data.x = (buf[0]<<8)|buf[1];
  gyro_data.y = (buf[2]<<8)|buf[3];
  gyro_data.z = (buf[4]<<8)|buf[5];

  // TRACE_DEBUG(2, TRACE_MPU9250, "read gyro " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(gyro_data.arr));
}

void mpu9250::read_accel(Vector3<int16_t>& accel_data)
{
  //consolep("MPU9250_READ_ACCEL()\n");
  I2C_ReadBuff(ACCEL_ADDRESS, ACCEL_XOUT_H, 6, ::buf);
  accel_data.x = (buf[0]<<8)|buf[1];
  accel_data.y = (buf[2]<<8)|buf[3];
  accel_data.z = (buf[4]<<8)|buf[5];
  // TRACE_DEBUG(4, TRACE_MPU9250, "read accel " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(accel_data.arr));
}

void mpu9250::read_mag(Vector3<int16_t>& mag_data)
{
  // ?????????????????
  I2C_WriteOneByte(GYRO_ADDRESS,0x37,0x02);//turn on Bypass Mode
  // sleep_ms(10);
  I2C_WriteOneByte(MAG_ADDRESS,0x0A,0x01);
  // sleep_ms(10);
  static Vector3<> mag;


  uint8_t wia_resp = I2C_ReadOneByte(MAG_ADDRESS, WIA_AD);

  if(wia_resp != WIA_VAL)
  {
    // PRINT("MAGNETOMETR FAILED\n");
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
  // TRACE_DEBUG(3, TRACE_MPU9250, "read mag " FORMAT_CREATE_TEMPLATE("%" PRIi16) " \n", INS_ARR(mag_data.arr));
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
  I2C_WriteOneByte(GYRO_ADDRESS,PWR_MGMT_1, 0x00);
  I2C_WriteOneByte(GYRO_ADDRESS, SMPLRT_DIV, 0x07);
  I2C_WriteOneByte(GYRO_ADDRESS, CONFIG, 0x06);
  I2C_WriteOneByte(GYRO_ADDRESS, GYRO_CONFIG, 0x10);
  I2C_WriteOneByte(GYRO_ADDRESS, ACCEL_CONFIG, 0x01);

  // config interrupts
	I2C_WriteOneByte(GYRO_ADDRESS, INT_BYPASS_CONFIG_AD, 0x10);
	I2C_WriteOneByte(GYRO_ADDRESS, INT_ENABLE_AD, 0x01);
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


