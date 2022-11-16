/**
  ******************************************************************************
  * @file    IMU.c
  * @author  Waveshare Team
  * @version V1.0
  * @date    29-August-2014
  * @brief   This file provides all the IMU firmware functions.

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
  ******************************************************************************
  */



//#include "IMU.h"
#include <math.h>
#include <stdio.h>
// #include "console/console.h"
#include "i2c.h"

uint8_t u8PressureType;

/**
  * @brief  invSqrt
  * @param
  * @retval
  */

float invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;

	long i = *(long*)&y;                //get bits for floating value
	i = 0x5f3759df - (i >> 1);          //gives initial guss you
	y = *(float*)&i;                    //convert bits back to float
	y = y * (1.5f - (halfx * y * y));   //newtop step, repeating increases accuracy

	return y;
}


/**
  * @brief  initializes IMU
  * @param  None
  * @retval None
  */
void IMU_Init(void)
{
	I2C_Init();
  //printf("IMU_Init()\n");
  uint8_t u8Ret;
  // mpu9250::init();
  bmp280::init();
  //scan();

}


