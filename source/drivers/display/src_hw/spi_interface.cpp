
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"


// c/c++ includes
#include <stdint.h>

// my includes
#include "spi_interface.hpp"
#include "traces.h"

// #------------------------------#
// |           macros             |
// #------------------------------#
#define SPI_PORT spi1
#define RST 12
#define DC 8
#define BL 13

#define CS 9
#define CLK 10
#define MOSI 11

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
// Back light
static uint lcd_bl_level;

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
uint8_t spi::init(void)
{
    // set pin
    gpio_init(CS);
    gpio_init(RST);
    gpio_init(DC);
    gpio_init(BL);

    gpio_set_dir(BL, GPIO_OUT);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_set_dir(RST, GPIO_OUT);
    gpio_set_dir(DC, GPIO_OUT);

    gpio_put(RST, 1);
    gpio_put(CS, 1);
    gpio_put(BL, 1);

    spi_init(SPI_PORT, 60 * 1000 * 1000); // DEFAULT: 8000000

    gpio_set_function(CLK, GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);

    // PWM Config
    gpio_set_function(BL, GPIO_FUNC_PWM);
    lcd_bl_level = pwm_gpio_to_slice_num(BL);
    pwm_set_wrap(lcd_bl_level, 100);
    pwm_set_chan_level(lcd_bl_level, PWM_CHAN_B, 1);
    pwm_set_clkdiv(lcd_bl_level, 50);
    pwm_set_enabled(lcd_bl_level, true);

    return 0;
}



void spi::set_brightness(uint8_t Value)
{
    if (Value > 100)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"DEV_SET_PWM Error \r\n");
    }
    else
    {
        pwm_set_chan_level(lcd_bl_level, PWM_CHAN_B, Value);
    }
}

void spi::writeRegister(uint8_t Reg)
{
    gpio_put(DC, 0);
    gpio_put(CS, 0);
    spi_write_blocking(SPI_PORT, &Reg, 1);
    gpio_put(CS, 1);
}

void spi::writeData(uint8_t Data)
{
    gpio_put(DC, 1);
    gpio_put(CS, 0);
    spi_write_blocking(SPI_PORT, &Data, 1);
    gpio_put(CS, 1);
}

void spi::writeDataBuffer(const uint8_t *Data, size_t length)
{
    gpio_put(DC, 1);
    gpio_put(CS, 0);
    spi_write_blocking(SPI_PORT, Data, length);
    gpio_put(CS, 1);
}

void spi::reset(void)
{
    gpio_put(RST, 1);
    sleep_ms(100);
    gpio_put(RST, 0);
    sleep_ms(100);
    gpio_put(RST, 1);
    sleep_ms(100);
}


// #------------------------------#
// | static functions definitions |
// #------------------------------#
