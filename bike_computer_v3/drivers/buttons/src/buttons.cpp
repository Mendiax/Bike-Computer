
#include "buttons/buttons.h"




// static declarations

//default function for interrupts
static void blank();

// global definitions

interrupt button0 = {BTN0, blank, GPIO_IRQ_EDGE_FALL};
// top right
interrupt button1 = {BTN1, blank, GPIO_IRQ_EDGE_FALL};
interrupt button1rel = {BTN1, blank, GPIO_IRQ_EDGE_RISE};
// bottom right
interrupt button2 = {BTN2, blank, GPIO_IRQ_EDGE_FALL};
interrupt button3 = {BTN3, blank, GPIO_IRQ_EDGE_FALL};

static void blank()
{

}
