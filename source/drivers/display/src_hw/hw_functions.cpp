
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes
#include "pico/stdlib.h"


// c/c++ includes

// my includes
#include "hw_functions.hpp"
#include "pico/time.h"
#include "spi_interface.hpp"


// #------------------------------#
// |           macros             |
// #------------------------------#

// TODO better asserts
#define assert_loop(x) if(!(x)){while (1) {sleep_ms(1000);}}


// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#
static void set_window(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
static void set_brightnes(uint8_t Value);
static void init_regs(void);


// #------------------------------#
// | global function definitions  |
// #------------------------------#
void display_init(void)
{
        static bool displayInited;
    if(displayInited) // check for multiple initializations
    {
        return;
    }
    displayInited = true;
    spi::init();
    // Hardware reset
    set_brightnes(50);

    spi::reset();

    // Set the read / write scan direction of the frame memory
    spi::writeRegister(0x36);  // MX, MY, RGB mode
#if 1                     // Horizontal
    spi::writeData(0X70); // 0x08 set RGB
#else
    spi::writeData(0X00); // 0x08 set RGB
#endif

    // Set the initialization register
    init_regs();
    sleep_ms(200);
}
void display_display(uint8_t display_buffer[DISPLAY_BUFFER_SIZE])
{
    set_window(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT); // DISPLAY_HEIGHT
    spi::writeRegister(0x2C); // memory write
    spi::writeDataBuffer(display_buffer, DISPLAY_BUFFER_SIZE);
    spi::writeRegister(0x29);
}

static void init_regs(void)
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
        | (RGB_ << 3)
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

void set_brightnes(uint8_t value)
{
    spi::set_brightness(value);
}

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
void set_window(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
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

// #------------------------------#
// | static functions definitions |
// #------------------------------#
