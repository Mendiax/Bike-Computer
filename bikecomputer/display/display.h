#ifndef __display_H__
#define __display_H__

#include "screen.h"

enum Display_type
{
    plot,
    lastVal
};

typedef struct Display
{
    Display_type current_type;
    RingBuffer
};

Display _Display;


void Display_init(){
    Screen_Setup();
}

void Display_setDisplayType(Display_type type){
    _Display.current_type = type;
}

#endif