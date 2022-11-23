#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "display/driver.hpp"
#include "display/debug.h"
#include "traces.h"


// SPI setup
#define SPI_PORT spi1
#define RST 12
#define DC 8
#define BL 13

#define CS 9
#define CLK 10
#define MOSI 11

// buffer setup
// 12bit per color so
// reg 3Ah
#define INPUT_COLORS 2
#if INPUT_COLORS == 0 // 5-6-5 bit
#define REG_3Ah 0x05
#define PIXEL_SIZE 16
#elif INPUT_COLORS == 1 // 6-6-6 bit
#define REG_3Ah 0x06
#define PIXEL_SIZE 18
#elif INPUT_COLORS == 2 // 4-4-4 bit
#define REG_3Ah 0x03
#define PIXEL_SIZE 12
#endif

// 240 * 320 * 12 / 8 = 115 200
#define DISPLAY_BUFFER_SIZE (DISPLAY_HEIGHT * DISPLAY_WIDTH * PIXEL_SIZE / 8) // just for safety
uint8_t display_buffer[DISPLAY_BUFFER_SIZE] = {0};

// Back light
uint lcd_bl_level;

// static def
/********************************************************************************
function:   Set the display Window(Xstart, Ystart, Xend, Yend)
parameter:
        xStart :   X direction Start coordinates
        Ystart :   Y direction Start coordinates
        Xend   :   X direction end coordinates
            (Parameter range: 0 < XS [15:0] < XE [15:0] < 239 (00Efh)): MV=”0”)
            (Parameter range: 0 < XS [15:0] < XE [15:0] < 319 (013Fh)): MV=”1”)
        Yend   :   Y direction end coordinates
            (Parameter range: 0 < YS [15:0] < YE [15:0] < 319 (013fh)): MV=”0”)
            (Parameter range: 0 < YS [15:0] < YE [15:0] < 239 (00EFh)): MV=”1”)
********************************************************************************/
void OLED_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void assert_loop(bool b);

void assert_loop(bool b)
{
    if (!b)
    {
        while (1)
        {
            PRINTF("ASSERT failed\n");
            sleep_ms(1000);
        }
    }
}

namespace spi
{
    uint8_t init(void);
    void writeData(uint8_t Data);
    void writeRegister(uint8_t Reg);
    void writeDataBuffer(const uint8_t *Data, size_t length);
}

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
void spi::writeRegister(uint8_t Reg)
{
    gpio_put(DC, 0); // OLED_DC_0;
    gpio_put(CS, 0); // OLED_CS_0;
    spi_write_blocking(SPI_PORT, &Reg, 1);
    gpio_put(CS, 1); // OLED_CS_1;
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

/*******************************************************************************
function:
        Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{

    // MY Bit D7- Page Address Order
    //     “0” = Top to Bottom (When MADCTL D7=”0”).
    //     “1” = Bottom to Top (When MADCTL D7=”1”).
    // MX Bit D6- Column Address Order
    //     “0” = Left to Right (When MADCTL D6=”0”).
    //     “1” = Right to Left (When MADCTL D6=”1”).
    // MV Bit D5- Page/Column Order
    //     “0” = Normal Mode (When MADCTL D5=”0”).
    //     “1” = Reverse Mode (When MADCTL D5=”1”)
    //     Note: Bits D7 to D5, alse refer to section 8.12 Address Control
    // ML Bit D4- Line Address Order
    //     “0” = LCD Refresh Top to Bottom (When MADCTL D4=”0”)
    //     “1” = LCD Refresh Bottom to Top (When MADCTL D4=”1”)
    // RGB Bit D3- RGB/BGR Order
    //     “0” = RGB (When MADCTL D3=”0”)
    //     “1” = BGR (When MADCTL D3=”1”)
    // MH Bit D2- Display Data Latch Data Order
    //     “0” = LCD Refresh Left to Right (When MADCTL D2=”0”)
    //     “1” = LCD Refresh Right to Left (When MADCTL D2=”1”)

    DEBUG_OLED("ini reg\n");


    // struct Setup
    // {
    //     union
    //     {
    //         uint8_t x36;
    //         struct
    //         {
    //             uint8_t MY : 1;
    //             uint8_t MX : 1;
    //             uint8_t MV : 1;
    //             uint8_t ML : 1;
    //             uint8_t RGB : 1;
    //             uint8_t MH : 1;
    //             uint8_t pad : 2;
    //         };
    //     };
    // };

    // Setup MADCTL = {0};
    // // horizonstal
    // MADCTL

    spi::writeRegister(0x36); // MY  MX  MV  ML RGB  MH  --  --
                         //  0   0   0   0   0   0   0   0
    const uint8_t glob = 0x0
        | (MY << 7)
        | (MX << 6)
        | (MV << 5)
        | (ML << 4)
        | (RGB << 3)
        | (MH << 2);
    spi::writeData(glob);

    spi::writeRegister(0x3A);     // rgb mode 0
    spi::writeData(REG_3Ah); // 0b101

    spi::writeRegister(0x21);

    spi::writeRegister(0x2A);
    spi::writeData(0x00);
    spi::writeData(0x00);
    spi::writeData(0x01);
    spi::writeData(0x3F);

    spi::writeRegister(0x2B);
    spi::writeData(0x00);
    spi::writeData(0x00);
    spi::writeData(0x00);
    spi::writeData(0xEF);

    spi::writeRegister(0xB2);
    spi::writeData(0x0C);
    spi::writeData(0x0C);
    spi::writeData(0x00);
    spi::writeData(0x33);
    spi::writeData(0x33);

    spi::writeRegister(0xB7);
    spi::writeData(0x35);

    spi::writeRegister(0xBB);
    spi::writeData(0x1F);

    spi::writeRegister(0xC0); //  LCM Control
    spi::writeData(0x2C);

    spi::writeRegister(0xC2);
    spi::writeData(0x01);

    spi::writeRegister(0xC3);
    spi::writeData(0x12);

    spi::writeRegister(0xC4);
    spi::writeData(0x20);

    spi::writeRegister(0xC6);  // Frame Rate Control in Normal Mode // coś z frame rate ale idk na razie
    spi::writeData(0x0F); // column inversion. idk ale jest jeszcez 0x0  dot inversion.

    spi::writeRegister(0xD0); //  Power Control 1 ?????
    spi::writeData(0xA4);
    spi::writeData(0xA1);

    spi::writeRegister(0xE0); //  Positive Voltage Gamma Control ???????????
    spi::writeData(0xD0);
    spi::writeData(0x08);
    spi::writeData(0x11);
    spi::writeData(0x08);
    spi::writeData(0x0C);
    spi::writeData(0x15);
    spi::writeData(0x39);
    spi::writeData(0x33);
    spi::writeData(0x50);
    spi::writeData(0x36);
    spi::writeData(0x13);
    spi::writeData(0x14);
    spi::writeData(0x29);
    spi::writeData(0x2D);

    spi::writeRegister(0xE1); // Negative Voltage Gamma Control  ??????
    spi::writeData(0xD0);
    spi::writeData(0x08);
    spi::writeData(0x10);
    spi::writeData(0x08);
    spi::writeData(0x06);
    spi::writeData(0x06);
    spi::writeData(0x39);
    spi::writeData(0x44);
    spi::writeData(0x51);
    spi::writeData(0x0B);
    spi::writeData(0x16);
    spi::writeData(0x14);
    spi::writeData(0x2F);
    spi::writeData(0x31);
    spi::writeRegister(0x21);

    spi::writeRegister(0x11);

    spi::writeRegister(0x29); // turn on
}

void display_set_brightnes(uint8_t Value)
{
    if (Value < 0 || Value > 100)
    {
        PRINTF("DEV_SET_PWM Error \r\n");
    }
    else
    {
        pwm_set_chan_level(lcd_bl_level, PWM_CHAN_B, Value);
    }
}

uint8_t spi::init(void)
{
    DEBUG_OLED("system init\n");

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

    // spi_init(SPI_PORT, 8000000); // SPI_CLOCK_DIV2 -> 8Mhz
    spi_init(SPI_PORT, 60 * 1000 * 1000); // OC

    // gpio_set_function(MISO, GPIO_FUNC_SPI);
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

/********************************************************************************
function:
            initialization
********************************************************************************/
void display::init(void)
{
    static bool displayInited;
    if(displayInited) // check for multiple initializations
    {
        return;
    }
    displayInited = true;
    DEBUG_OLED("1in5_Init\n");
    spi::init();
    // Hardware reset
    display_set_brightnes(50);

    OLED_Reset();

    // Set the read / write scan direction of the frame memory
    spi::writeRegister(0x36);  // MX, MY, RGB mode
#if 1                     // Horizontal
    spi::writeData(0X70); // 0x08 set RGB
#else
    spi::writeData(0X00); // 0x08 set RGB
#endif

    // Set the initialization register
    OLED_InitReg();
    sleep_ms(200);

    // Turn on the OLED display
    // spi::writeRegister(0xAF);
    // display::clear();
}


void OLED_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    // if ((Xstart > DISPLAY_WIDTH) || (Ystart > DISPLAY_HEIGHT) ||
    //     (Xend > DISPLAY_WIDTH) || (Yend > DISPLAY_HEIGHT))
    //     return;

    // spi::writeRegister(0x15);
    // spi::writeRegister(Xstart / 2);
    // spi::writeRegister(Xend / 2 - 1);

    // spi::writeRegister(0x75);
    // spi::writeRegister(Ystart);
    // spi::writeRegister(Yend - 1);
    #if MV == 0
        assert_loop(Xend <= 240);
        assert_loop(Yend <= 320);
    #else
        assert_loop(Xend <= 320);
        assert_loop(Yend <= 240);
    #endif

    // set the X coordinates
    spi::writeRegister(0x2A); // column set address

    // x start
    spi::writeData(Xstart >> 8);
    spi::writeData(Xstart & 0xff);

    // x end
    spi::writeData((Xend - 1) >> 8);
    spi::writeData((Xend - 1) & 0xFF);

    // set the Y coordinates
    spi::writeRegister(0x2B);
    // y start
    spi::writeData(Ystart >> 8);
    spi::writeData(Ystart & 0xff);
    // y end
    spi::writeData((Yend - 1) >> 8);
    spi::writeData((Yend - 1) & 0xff);
}

void display::set_pixel_row(const uint_fast32_t idx, uint_fast32_t width, const display::DisplayColor color)
{
    if ((idx + width) > DISPLAY_PIXEL_COUNT)
    {
        DEBUG_OLED("[ERROR] display::setPixel %" PRIuFAST32 " > DISPLAY_PIXEL_COUNT\n", idx);
        return;
    }
    if (width == 0)
    {
        DEBUG_OLED("[ERROR] display::setPixel width cannot be 0. width == %" PRIuFAST32 " \n", width);
        return;
    }
    struct pixel2
    {
        uint8_t rg;
        uint8_t br;
        uint8_t gb;
    };
    pixel2 *buffer_pixels = (pixel2 *)::display_buffer;

    uint_fast32_t index_pixels = idx >> 1;
    pixel2 * pixel_p = &buffer_pixels[index_pixels];

    // align in memory if needed
    if (idx & 1)
    {
        pixel_p->br = (pixel_p->br & 0xf0) | (color.r & 0x0f);
        pixel_p->gb = (color.g << 4) | (color.b & 0x0f);
        ++pixel_p;
        --width;
    }
    uint_fast32_t pixels_left = width >> 1;
    pixel2 filled_pixel;
    filled_pixel.rg = (color.r << 4) | color.g;
    filled_pixel.br = (color.b << 4) | color.r;
    filled_pixel.gb = (color.g << 4) | color.b;

    // file 2 pixels at time
    while (pixels_left > 0)
    {
        *pixel_p = filled_pixel;
        ++pixel_p;
        --pixels_left;
    }

    // align end of line
    if(width & 1)
    {
        pixel_p->rg = (color.r << 4) | (color.g & 0x0f);
        pixel_p->br = (color.b << 4) | (pixel_p->br & 0x0f);
    }
}


void display::setPixel(const uint_fast32_t idx, const display::DisplayColor color)
{
    // assert_loop((idx) < DISPLAY_PIXEL_COUNT);
    if ((idx) > DISPLAY_PIXEL_COUNT)
    {
        DEBUG_OLED("[ERROR] display::setPixel %" PRIuFAST32 " > DISPLAY_PIXEL_COUNT\n", idx);
        return;
    }
    struct pixel2
    {
        uint8_t rg;
        uint8_t br;
        uint8_t gb;

    };
    pixel2 *buffer_pixels = (pixel2 *)::display_buffer;

    const uint_fast32_t index_pixels = idx >> 1;
    pixel2 *const pixel_p = &buffer_pixels[index_pixels];
    if (idx & 1)
    {
        pixel_p->br = (pixel_p->br & 0xf0) | (color.r & 0x0f);
        pixel_p->gb = (color.g << 4) | (color.b & 0x0f);
    }
    else
    {
        pixel_p->rg = (color.r << 4) | (color.g & 0x0f);
        pixel_p->br = (color.b << 4) | (pixel_p->br & 0x0f);
    }
}

/********************************************************************************
function:
            Clear screen
********************************************************************************/
void display::clear(void)
{
    DEBUG_OLED("clear\n");
    // #if MV == 1
    //     OLED_SetWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    // #else
    //     OLED_SetWindow(0, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH);
    // #endif

    // display::DisplayColor clearColor(0x0,0x0,0x0);
    // for(uint32_t i =0; i < DISPLAY_PIXEL_COUNT; i++)
    //{
    //     display::setPixel(i, clearColor);
    // }
    memset(::display_buffer, 0x0, DISPLAY_BUFFER_SIZE);
    //display::display();
}

void display::fill(const display::DisplayColor color)
{
    // 07:39:51.205 -> [DEBUG_OLED] : FILL 38400 sus
    DEBUG_OLED("FILL");
    for (uint_fast32_t i = 38000; i < DISPLAY_PIXEL_COUNT; i++)
    {
        display::setPixel(i, color);
    }
    DEBUG_OLED("FILL2\n");
    // memset(display_buffer, 0xff, DISPLAY_BUFFER_SIZE / 2);

    //display::display();
}

void display::fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const display::DisplayColor color)
{

}


/********************************************************************************
function:   Update all memory to OLED
********************************************************************************/
void display::display()
{
    OLED_SetWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT); // DISPLAY_HEIGHT
    spi::writeRegister(0x2C); // memory write
    spi::writeDataBuffer(::display_buffer, DISPLAY_BUFFER_SIZE);
    spi::writeRegister(0x29);
    // for(uint32_t i = 0; i < DISPLAY_BUFFER_SIZE; i += 3)
    //{
    //     int r,g,b,r2,g2,b2;
    //     r   = (display_buffer[i    ] & 0x0f) >> 0;
    //     g   = (display_buffer[i    ] & 0xf0) >> 4;
    //     b   = (display_buffer[i + 1] & 0x0f) >> 0;
    //     r2  = (display_buffer[i + 1] & 0xf0) >> 4;
    //     g2  = (display_buffer[i + 2] & 0x0f) >> 0;
    //     b2  = (display_buffer[i + 2] & 0xf0) >> 4;
    //     //PRINTF("%d %d %d %d %d %d \n",  r,g,b,r2,g2,b2);
    // }
}
