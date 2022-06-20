#include "OLED_Driver.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"


#include <OLED_Debug.h>

/*******************************************************************************
function:
            Hardware reset
*******************************************************************************/
static void OLED_Reset(void)
{
    DEBUG_OLED("reset\n");
    gpio_put(RST, 1);
    sleep_ms(100);
    gpio_put(RST, 0);
    sleep_ms(100);
    gpio_put(RST, 1);
    sleep_ms(100);
}

/*******************************************************************************
function:
            Write register address and data
*******************************************************************************/
void OLED_WriteReg(uint8_t Reg)
{
    gpio_put(DC, 0); // OLED_DC_0;
    gpio_put(CS, 0); // OLED_CS_0;
    spi_write_blocking(SPI_PORT, &Reg, 1);
    gpio_put(CS, 1); // OLED_CS_1;
}

void OLED_WriteData(uint8_t Data)
{
    gpio_put(DC, 1);
    gpio_put(CS, 0);
    spi_write_blocking(SPI_PORT, &Data, 1);
    gpio_put(CS, 1);
}

/*******************************************************************************
function:
        Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{
    DEBUG_OLED("init reg\n");
    OLED_WriteReg(0xae); //--turn off oled panel

    OLED_WriteReg(0x15); //   set column address
    OLED_WriteReg(0x00); //  start column   0
    OLED_WriteReg(0x7f); //  end column   127

    OLED_WriteReg(0x75); //   set row address
    OLED_WriteReg(0x00); //  start row   0
    OLED_WriteReg(0x7f); //  end row   127

    OLED_WriteReg(0xa0); // gment remap
    OLED_WriteReg(0x51); // 51

    OLED_WriteReg(0xa1); // start line
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xa2); // display offset
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xAF);
}

uint8_t System_Init(void)
{
    DEBUG_OLED("system init\n");
    gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_init(RST);
    gpio_set_dir(RST, GPIO_OUT);
    gpio_init(DC);
    gpio_set_dir(DC, GPIO_OUT);

    gpio_put(RST, 1);
    gpio_put(CS, 1);

    // set pin
    gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_init(RST);
    gpio_set_dir(RST, GPIO_OUT);
    gpio_init(DC);
    gpio_set_dir(DC, GPIO_OUT);

    gpio_put(RST, 1);
    gpio_put(CS, 1);

    spi_init(SPI_PORT, 8000000); // SPI_CLOCK_DIV2 -> 8Mhz

    spi_set_format(SPI_PORT,   // SPI instance
                   8,          // Number of bits per transfer
                   SPI_CPOL_1, // Polarity (CPOL)
                   SPI_CPHA_1, // Phase (CPHA)
                   SPI_MSB_FIRST);

    gpio_set_function(MISO, GPIO_FUNC_SPI);
    gpio_set_function(CLK, GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);

    return 0;
}

/********************************************************************************
function:
            initialization
********************************************************************************/
void OLED_1in5_Init(void)
{
    DEBUG_OLED("1in5_Init\n");
    System_Init();
    // Hardware reset
    OLED_Reset();

    // Set the initialization register
    OLED_InitReg();
    sleep_ms(200);

    // Turn on the OLED display
    OLED_WriteReg(0xAF);
    OLED_1in5_Clear();
}

/********************************************************************************
function:   Set the display Window(Xstart, Ystart, Xend, Yend)
parameter:
        xStart :   X direction Start coordinates
        Ystart :   Y direction Start coordinates
        Xend   :   X direction end coordinates
        Yend   :   Y direction end coordinates
********************************************************************************/
void OLED_SetWindow(UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend)
{
    if ((Xstart > OLED_1in5_WIDTH) || (Ystart > OLED_1in5_HEIGHT) ||
        (Xend > OLED_1in5_WIDTH) || (Yend > OLED_1in5_HEIGHT))
        return;

    OLED_WriteReg(0x15);
    OLED_WriteReg(Xstart / 2);
    OLED_WriteReg(Xend / 2 - 1);

    OLED_WriteReg(0x75);
    OLED_WriteReg(Ystart);
    OLED_WriteReg(Yend - 1);
}

/********************************************************************************
function:
            Clear screen
********************************************************************************/
void OLED_1in5_Clear(void)
{
    UWORD i;
    OLED_SetWindow(0, 0, 128, 128);
    for (i = 0; i < OLED_1in5_WIDTH * OLED_1in5_HEIGHT / 2; i++)
    {
        OLED_WriteData(0x00);
    }
}

void OLED_1in5_Fill(void)
{
    UWORD i;
    OLED_SetWindow(0, 0, 128, 128);
    for (i = 0; i < OLED_1in5_WIDTH * OLED_1in5_HEIGHT / 2; i++)
    {
        OLED_WriteData(0xff);
    }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_1in5_Display(const UBYTE *Image)
{
    UWORD i, j, temp;
    OLED_SetWindow(0, 0, 128, 128);
    for (i = 0; i < OLED_1in5_HEIGHT; i++)
        for (j = 0; j < OLED_1in5_WIDTH / 2; j++)
        {
            temp = Image[j + i * 64];
            OLED_WriteData(temp);
        }
}

/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void OLED_1in5_Display_Part(const UBYTE *Image, UBYTE Xstart, UBYTE Ystart, UBYTE Xend, UBYTE Yend)
{
    UWORD i, j, temp;
    OLED_SetWindow(Xstart, Ystart, Xend, Yend);
    for (i = 0; i < Yend - Ystart; i++)
        for (j = 0; j < (Xend - Xstart) / 2; j++)
        {
            temp = Image[j + i * (Xend - Xstart) / 2];
            OLED_WriteData(temp);
        }
}
