#include "display/print.h"
#include "console/console.h"
#include <stdarg.h>

// const sFONT Font16 = {
//   Font16_Table,
//   11, /* Width */
//   16, /* Height */
// };



// my console
#define FONT Font16
/*Font16.height*/
#define FONT_HEIGHT (16)
#define FONT_WIDTH (11)
#define NO_LINES (DISPLAY_HEIGHT / FONT_HEIGHT)
#define LINE_HEIGHT (FONT_HEIGHT)
#define LINE_LENGTH (DISPLAY_WIDTH / FONT_WIDTH)
static char console[NO_LINES][LINE_LENGTH + 1];

static bool console_inited;

void consoleLogInit()
{
    for(int i = 0; i < NO_LINES; i++)
    {
        memset(console[i], 0, LINE_LENGTH);
    }
    /* LCD Init */
    display::init();
    display::clear();
    display::display();
    console_inited = true;
    consolep("LCD Console started\n");
}

void consoleScrollup()
{
    for(int i =  NO_LINES - 1; i > 0; i--)
    {
        memcpy(console[i], console[i - 1], LINE_LENGTH + 1);
    }
}

void consoleLog(const char* fmt, ...)
{
    if(!console_inited)
    {
        return;
    }
    va_list args;
    va_start(args, fmt);
    enum{bufferSize=256};
    char preBuffer[bufferSize] = {0};
    snprintf(preBuffer, bufferSize, fmt, args);
    preBuffer[bufferSize-1] = '\0';

    const char* arrPtr = &preBuffer[0];

    while(strlen(arrPtr) > 0)
    {
        consoleScrollup();
        memcpy(console[0], arrPtr, LINE_LENGTH);
        arrPtr += LINE_LENGTH;
    }

    display::clear();
    for(int i = 0; i < NO_LINES; i++)
    {
        if(console[i] == NULL)
        {
            break;
        }
        // if(console[i] == '\n' || console[i] == '\r')
        // {
        //     continue;
        // }
        Paint_Println(0, DISPLAY_HEIGHT - (LINE_HEIGHT * (i + 1)), console[i], &FONT);
    }
    display::display();
    va_end(args);
}