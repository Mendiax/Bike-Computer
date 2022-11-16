/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//Magnetometer Registers
#define AK8963_ADDRESS   0x0C<<1
#define AK8963_WHO_AM_I  0x00 // should return 0x48
#define AK8963_INFO      0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define MPU9250_ADDRESS  0x68<<1
#define INT_PIN_CFG      0x37

#define DATA_READY_MASK 0x01
#define MAGIC_OVERFLOW_MASK 0x8

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

float MagX;
float MagY;
float MagZ;

/* USER CODE BEGIN PV */
enum Mscale {
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS      // 0.15 mG per LSB
};

uint8_t Mscale = MFS_16BITS; // Choose either 14-bit or 16-bit magnetometer resolution
uint8_t Mmode = 0x02;

const uint16_t i2c_timeoutB = 100;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/


float MagnetoX;
float MagnetoY;
float MagnetoZ;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */


  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */


  /* USER CODE BEGIN 2 */
    printf("**************************** \r\n");
    printf("MPU9250 STM32 Implementation \r\n");
    printf("**************************** \r\n");

  //pre-def. vars
    uint8_t readData;
    uint8_t writeData;

    // Check MPU and Mag---------------------------------------------------------------------------------------------------
    //read MPU9255 WHOAMI
    HAL_I2C_Mem_Read(&hi2c2, MPU9250_ADDRESS, WHO_AM_I_MPU9250, 1, &readData, 1, i2c_timeoutB);
    printf("MPU WHO AM I is (Must return 113): %d\r\n", readData);

    //enable Mag bypass
    writeData = 0x22;
    HAL_I2C_Mem_Write(&hi2c2, MPU9250_ADDRESS, INT_PIN_CFG, 1, &writeData, 1, i2c_timeoutB);

    //read AK8963 WHOAMI
    HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDRESS, AK8963_WHO_AM_I, 1, &readData, 1, i2c_timeoutB);
    printf("MAG WHO AM I is (Must return 72): %d\r\n", readData);

    printf("------------------------------------------------\r\n");




    //Init Mag-------------------------------------------------------------------------------------------------------------
    //Power down magnetometer
    writeData = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDRESS, AK8963_CNTL, 1, &writeData, 1, i2c_timeoutB);
    HAL_Delay(100);

    //Enter Fuse ROM access mode
    writeData = 0x0F;
    HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDRESS, AK8963_CNTL, 1, &writeData, 1, i2c_timeoutB);
    HAL_Delay(100);

    //Read the x-, y-, and z-axis calibration values
    uint8_t rawMagCalData[3];
    HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDRESS, AK8963_ASAX, 1, &rawMagCalData[0], 3, i2c_timeoutB);
    float calMagX =  (float)(rawMagCalData[0] - 128)/256. + 1.;   // Return x-axis sensitivity adjustment values, etc.
    float calMagY =  (float)(rawMagCalData[1] - 128)/256. + 1.;
    float calMagZ =  (float)(rawMagCalData[2] - 128)/256. + 1.;

    printf("Mag cal off X: %f\r\n", calMagX);
    printf("Mag cal off Y: %f\r\n", calMagY);
    printf("Mag cal off Z: %f\r\n", calMagZ);
    HAL_Delay(100);

    //Power down magnetometer
    writeData = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDRESS, AK8963_CNTL, 1, &writeData, 1, i2c_timeoutB);
    HAL_Delay(100);

    //Set magnetometer data resolution and sample ODR
    writeData = Mscale << 4 | 0x02;
    //writeData = 0x16;
    printf("writeData: %d\r\n", writeData);
    HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDRESS, AK8963_CNTL, 1, &writeData, 1, i2c_timeoutB);
    HAL_Delay(100);


    printf("------------------------------------------------\r\n");


    /*
    //Read Mag-------------------------------------------------------------------------------------------------------------
    //Read Mag data
    uint8_t rawMagData[6];
    HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDRESS, AK8963_XOUT_L, 1, &rawMagData[0], 6, i2c_timeoutB);
    MagX = ((int16_t)rawMagData[1] << 8) | rawMagData[0];
    MagY = ((int16_t)rawMagData[3] << 8) | rawMagData[2];
    MagZ = ((int16_t)rawMagData[5] << 8) | rawMagData[4];

    printf("Mag X: %f\r\n", MagX);
    printf("Mag Y: %f\r\n", MagY);
    printf("Mag Z: %f\r\n", MagZ);
    HAL_Delay(100);

    printf("------------------------------------------------\r\n");
    */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

        HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDRESS, AK8963_ST1, 1, &readData, 1, i2c_timeoutB);
        printf("Read Data: %d\r\n", readData);
        printf("Read Data2: %d\r\n", (readData & 0x01));
        if( (readData & 0x01) == 0x01 ){

            //Read Mag data
            uint8_t rawMagData[7];
            HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDRESS, AK8963_XOUT_L, 1, &rawMagData[0], 7, i2c_timeoutB);
            uint8_t c = rawMagData[6];

            if(!(c & 0x08)) {
                MagX = ((int16_t)rawMagData[1] << 8) | rawMagData[0];
                MagY = ((int16_t)rawMagData[3] << 8) | rawMagData[2];
                MagZ = ((int16_t)rawMagData[5] << 8) | rawMagData[4];

                //Print to Com port via STLINK
                printf("Mag X: %f\r\n", MagX);
                printf("Mag Y: %f\r\n", MagY);
                printf("Mag Z: %f\r\n", MagZ);
                printf("------------------------------------------------\r\n");
            }

        }
        else {
            printf("No Data? \r\n");
            printf("------------------------------------------------\r\n");
        }
        HAL_Delay(500);

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}