#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "display/driver.hpp"
#include "traces.h"
#include <cstddef>


// SPI setup
#define SPI_PORT spi1
#define RST 12
#define DC 8
#define BL 13

#define CS 9
#define CLK 10
#define MOSI 11

// SETUP
#ifdef ROTATE
    // VERITCAL
    #define MY 1
    #define MX 1
    #define MV 0
#else
    // HORIZONTAL
    #define MY 0
    #define MX 1
    #define MV 1
#endif
    #define ML 0
    #define RGB 0
    #define MH 1

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
#define DISPLAY_BUFFER_SIZE (DISPLAY_HEIGHT * DISPLAY_WIDTH * PIXEL_SIZE / 8)
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
void lcd_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void assert_loop(bool b);

void assert_loop(bool b)
{
    if (!b)
    {
        while (1)
        {
            TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"ASSERT failed\n");
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

static void lcd_Reset(void)
{
    gpio_put(RST, 1);
    sleep_ms(100);
    gpio_put(RST, 0);
    sleep_ms(100);
    gpio_put(RST, 1);
    sleep_ms(100);
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

static void lcd_InitReg(void)
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


    spi::writeRegister(0x36);
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

    spi::writeRegister(0xC6);  // Frame Rate Control in Normal Mode
    spi::writeData(0x0F); // column inversion.

    spi::writeRegister(0xD0); //  Power Control 1
    spi::writeData(0xA4);
    spi::writeData(0xA1);

    spi::writeRegister(0xE0); //  Positive Voltage Gamma Control
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

    spi::writeRegister(0xE1); // Negative Voltage Gamma Control
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
    if (Value > 100)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"DEV_SET_PWM Error \r\n");
    }
    else
    {
        pwm_set_chan_level(lcd_bl_level, PWM_CHAN_B, Value);
    }
}

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

void display::init(void)
{
    static bool displayInited;
    if(displayInited) // check for multiple initializations
    {
        return;
    }
    displayInited = true;
    spi::init();
    // Hardware reset
    display_set_brightnes(50);

    lcd_Reset();

    // Set the read / write scan direction of the frame memory
    spi::writeRegister(0x36);  // MX, MY, RGB mode
#if 1                     // Horizontal
    spi::writeData(0X70); // 0x08 set RGB
#else
    spi::writeData(0X00); // 0x08 set RGB
#endif

    // Set the initialization register
    lcd_InitReg();
    sleep_ms(200);
}


void lcd_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
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
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"[ERROR] display::setPixel %" PRIuFAST32 " > DISPLAY_PIXEL_COUNT\n", idx);
        return;
    }
    if (width == 0)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"[ERROR] display::setPixel width cannot be 0. width == %" PRIuFAST32 " \n", width);
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
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, "[ERROR] display::setPixel %" PRIuFAST32 " > DISPLAY_PIXEL_COUNT\n", idx);
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

/**
 * @brief Clear screen
 *
 */
void display::clear(void)
{
    memset(::display_buffer, 0x0, DISPLAY_BUFFER_SIZE);
}


/**
 * @brief Update all memory to OLED
 *
 */
void display::display()
{
    lcd_SetWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT); // DISPLAY_HEIGHT
    spi::writeRegister(0x2C); // memory write
    spi::writeDataBuffer(::display_buffer, DISPLAY_BUFFER_SIZE);
    spi::writeRegister(0x29);
}


// static declaration
static void Paint_DrawCharGen(uint16_t x, uint16_t y, const char character,
                              const sFONT *font,
                              display::DisplayColor colorForeground = FONT_FOREGROUND,
                              display::DisplayColor colorBackground = FONT_BACKGROUND,
                              uint8_t scale = 1,
                              bool overwriteBackground = false);

static void Paint_PrintlnGen(uint16_t x, uint16_t y, const char *str,
                             const sFONT *font,
                             display::DisplayColor colorForeground = FONT_FOREGROUND,
                             display::DisplayColor colorBackground = FONT_BACKGROUND,
                             uint8_t scale = 1,
                             bool overwriteBackground = false);

static void Paint_Println_multilineGen(uint16_t x, uint16_t y, const char *str,
                                       const sFONT *font,
                                       display::DisplayColor colorForeground = FONT_FOREGROUND,
                                       display::DisplayColor colorBackground = FONT_BACKGROUND,
                                       uint8_t scale = 1,
                                       bool overwriteBackground = false);

void Paint_DrawLineGen(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void (*draw_func)(uint16_t x, uint16_t y, display::DisplayColor color), display::DisplayColor color, uint8_t scale);


// global definitions
void display::set_pixel(uint16_t x, uint16_t y, display::DisplayColor color)
{
    /*            width
    #---------------------------->x
    |
    |
    |
    | height
    |
    |
    |
   \/
    y
    */
    if(x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, "write outside of window x=%" PRIu16 " y=%" PRIu16 "\n", x, y);
        //return;
        // no return (XD) so it can be optimised out when compiling without traces and have greater performance
    }
    uint_fast32_t index = 0;
    index += y * (display::width);
    index += x;
    display::setPixel(index, color);
}

void display::draw_char(uint16_t x, uint16_t y, const char character,
                    const sFONT *font,
                    display::DisplayColor colorForeground,
                    uint8_t scale)
{
    Paint_DrawCharGen(x, y, character,
                      font,
                      colorForeground,
                      FONT_BACKGROUND,
                      scale,
                      false);
}

void display::println(uint16_t x, uint16_t y, const char *str,
                   const sFONT *font,
                   display::DisplayColor colorForeground,
                   uint8_t scale)
{
    Paint_PrintlnGen(x, y, str,
                     font,
                     colorForeground,
                     FONT_BACKGROUND,
                     scale,
                     false);
}

void display::draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, display::DisplayColor color, uint8_t scale)
{
    Paint_DrawLineGen(x0,y0,x1,y1, display::set_pixel, color, scale);
}

void Paint_DrawLineGen(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void (*draw_func)(uint16_t x, uint16_t y, display::DisplayColor color), display::DisplayColor color, [[maybe_unused]]uint8_t scale)
{
    int_fast32_t dx = std::abs((int32_t)x1 - (int32_t)x0);
    uint_fast8_t sx = x0 < x1 ? 1 : -1;
    int_fast32_t dy = - std::abs((int32_t)y1 - (int32_t)y0);
    uint_fast8_t sy = y0 < y1 ? 1 : -1;

    int_fast32_t error = dx + dy;

    while(true)
    {
        draw_func(x0, y0, color);
        if(x0 == x1 && y0 == y1)
            break;
        auto e2 = 2 * error;
        if (e2 >= dy)
        {
            if(x0 == x1)
                break;
            error = error + dy;
            x0 = x0 + sx;
        }

        if (e2 <= dx)
        {
            if (y0 == y1)
                break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}

static void Paint_PrintlnGen(uint16_t x, uint16_t y, const char *str,
                             const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale, bool overwriteBackground)
{
    TRACE_DEBUG(1, TRACE_DISPLAY_PRINT, "writing string \"%s\" \r\n", str);

    for (size_t charIdx = 0; charIdx < strlen(str); charIdx++)
    {
        if (x > display::width - font->width)
        {
            TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"line too long %s at %u", str, x);
            return;
        }
        Paint_DrawCharGen(x, y, str[charIdx], font, colorForeground, colorBackground, scale, overwriteBackground);
        x += font->width * scale;
    }
}

[[maybe_unused]]
static void Paint_Println_multilineGen(uint16_t x, uint16_t y, const char *str,
                                       const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale, bool overwriteBackground)
{
    TRACE_DEBUG(1, TRACE_DISPLAY_PRINT, "writing string \"%s\" \r\n", str);

    uint16_t startX = x;
    for (size_t charIdx = 0; charIdx < strlen(str); charIdx++)
    {
        if (x > display::width - font->width)
        {
            x = startX;
            y += font->height;
        }
        Paint_DrawCharGen(x, y, str[charIdx], font, colorForeground, colorBackground, scale, overwriteBackground);
        x += font->width;
    }
}

static void setpixelScale(uint_fast16_t xPoint, uint_fast16_t yPoint,  uint_fast16_t page, uint_fast16_t column, display::DisplayColor color, uint_fast8_t scale)
{
    uint_fast16_t x = xPoint + column * scale;
    uint_fast16_t y = yPoint + page * scale;

    for (uint_fast16_t p2 = 0; p2 < scale; p2++)
    {
        uint_fast32_t index = 0;
        index += (y + p2) * (display::width);
        index += x;
        display::set_pixel_row(index, scale, color);
    }
}

static void Paint_DrawCharGen(uint16_t xPoint, uint16_t yPoint, const char character,
                              const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale,
                              bool overwriteBackground)
{
    if(character < 32 || character > 126)
        return;

    TRACE_DEBUG(2, TRACE_DISPLAY_PRINT, "writing char \"%c\" scale %" PRIu8 "\r\n", character, scale);

    uint16_t page, column;
    if (xPoint > display::width || yPoint > display::height)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, " display::draw_char Input exceeds display range %" PRIu16 ",%" PRIu16 "\n", xPoint, yPoint);
        return;
    }

    size_t charOffset = (character - ' ') * font->height * (font->width / 8 + (font->width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[charOffset];

    for (page = 0; page < font->height; page++)
    {
        for (column = 0; column < font->width; column++)
        {
            if (*ptr & (0x80 >> (column % 8)))
            {
                setpixelScale(xPoint, yPoint, page, column, colorForeground, scale);
            }
            else if (overwriteBackground)
            {
                setpixelScale(xPoint, yPoint, page, column, colorBackground, scale);
            }
            // One pixel is 8 bits
            if (column % 8 == 7)
                ptr++;
        }
        if (font->width % 8 != 0)
            ptr++;
    }
}
