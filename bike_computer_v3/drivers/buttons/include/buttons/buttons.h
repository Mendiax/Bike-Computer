#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <interrupts/interrupts.hpp>

#define BTN0 15
#define BTN1 21
#define BTN2 2 // speed emulation
#define BTN3 3

extern interrupt button0;
extern interrupt button1;
extern interrupt button2;
extern interrupt button3;
#endif
