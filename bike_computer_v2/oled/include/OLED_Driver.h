#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H		

#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string.h>

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define LOW 0
#define HIGH 1

#define SPI_PORT spi0

#define RST 7
#define DC 6
#define CS   5
#define MISO 4 // not used
#define MOSI 3
#define CLK  2

#define OLED_1in5_HEIGHT 128
#define OLED_1in5_WIDTH 128

uint8_t System_Init(void);

void OLED_WriteData(uint8_t Data);
void OLED_WriteReg(uint8_t Reg);

void OLED_1in5_Init(void);

void OLED_1in5_Clear(void);
void OLED_1in5_Fill(void);

void OLED_SetWindow(UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend);

void OLED_1in5_Display(const UBYTE *Image);
void OLED_1in5_Display_Part(const UBYTE *Image, UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend);

class DisplayBuffer
{
public:
    UBYTE* buffer;
    uint8_t width;
    uint8_t height;
    uint16_t size;

    DisplayBuffer(int x, int y)
    {
        size  = x * y / 2;
        buffer = new uint8_t[size];
        memset(buffer, 0 , sizeof(uint8_t) * size);
        width = x;
        height = y;
    }
    ~DisplayBuffer()
    {
        delete buffer;
    }

    void write()
    {
        OLED_SetWindow(0,0,width, height);
        for(int i = size - 1; i >= 0; i--) // TODO do it better. differenbt set pixel ?
        {
            uint8_t chunk = (buffer[i] << 4) | (buffer[i] >> 4);
            OLED_WriteData(chunk);
        }
    }
    void clear()
    {
        memset(buffer, 0 , sizeof(uint8_t) * size);
    }
};

#endif

