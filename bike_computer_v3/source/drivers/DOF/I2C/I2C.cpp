/**
  ******************************************************************************
  * @file    I2C.c
  * @author  Waveshare Team
  * @version V1.0
  * @date    29-August-2014
  * @brief   This file provides all the I2C firmware functions.

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


#include "I2C.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
// #include "console/console.h"

#define I2C_PIN_SDA 18
#define I2C_PIN_SCL 19

/**
  * @brief  Initializes I2C
  * @param  None
  * @retval None
  */

void I2C_Init(void)
{
	bi_decl(bi_2pins_with_func(I2C_PIN_SDA, I2C_PIN_SCL, GPIO_FUNC_I2C));
	//stdio_init_all();

	i2c_init(i2c1, 100 * 1000);
  gpio_set_function(I2C_PIN_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_PIN_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_PIN_SDA);
  gpio_pull_up(I2C_PIN_SCL);
}

/**
  * @brief  Write an byte to the specified device address through I2C bus.
  *
  * @param DevAddr: The address byte of the slave device
  * @param RegAddr: The address byte of  register of the slave device
  * @param  Data: the data would be writen to the specified device address
  * @retval  None
**/

void I2C_WriteOneByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t Data)
{
	i2c_write_blocking(i2c1, DevAddr, &RegAddr, 1, true);  // true to keep master control of bus
}

/**
  * @brief Write a buffer specified sizes  to the specified device address through I2C bus.
  *
  * @param DevAddr: The address byte of the slave device
  * @param RegAddr: The address byte of  register of the slave device
  * @param Num: the sizes of the specified buffer
  * @param pBuff: point to a the specified buffer that would be writen
  * @retval  false: paramter error
  *                true: Write a buffer succeed
**/

bool I2C_WriteBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff)
{
	// uint8_t i;

	// if(0 == Num || NULL == pBuff)
	// {
	// 	return false;
	// }

	// I2C_Start();
	// I2C_WriteByte(DevAddr | I2C_Direction_Transmitter);
	// I2C_WaiteForAck();
	// I2C_WriteByte(RegAddr);
	// I2C_WaiteForAck();

	// for(i = 0; i < Num; i ++)
	// {
	// 	I2C_WriteByte(*(pBuff + i));
	// 	I2C_WaiteForAck();
	// }
	// I2C_Stop();

	return true;
}

/**
  * @brief Read an byte from the specified device address through I2C bus.
  *
  * @param DevAddr: The address byte of the slave device
  * @param RegAddr: The address byte of  register of the slave device
  *
  * @retval  the byte read from I2C bus
**/

uint8_t I2C_ReadOneByte(uint8_t DevAddr, uint8_t RegAddr)
{
	uint8_t TempVal = 0;

	i2c_write_blocking(i2c1, DevAddr, &RegAddr, 1, true);  // true to keep master control of bus
    // read in one go as register addresses auto-increment
  i2c_read_blocking(i2c1, DevAddr, &TempVal, 1, false);  // false, we're done reading

	return TempVal;
}

/**
  * @brief Read couples of bytes  from the specified device address through I2C bus.
  *
  * @param DevAddr: The address byte of the slave device
  * @param RegAddr: The address byte of  register of the slave device
  * @param Num: the sizes of the specified buffer
  * @param pBuff: point to a the specified buffer that would read bytes from I2C bus
  * @retval  false: paramter error
  *                true: read a buffer succeed
**/

bool I2C_ReadBuff(uint8_t DevAddr, uint8_t RegAddr, uint8_t Num, uint8_t *pBuff)
{
	i2c_write_blocking(i2c1, DevAddr, &RegAddr, 1, true);  // true to keep master control of bus
    // read in one go as register addresses auto-increment
  i2c_read_blocking(i2c1, DevAddr, pBuff, Num, false);  // false, we're done reading

	return true;
}

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int scan() {
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
        {
            ret = PICO_ERROR_GENERIC;
            printf("R");

        }
        else
        {
            ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);
            printf(ret < 0 ? "." : "@");

        }

        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
    return 0;
}

