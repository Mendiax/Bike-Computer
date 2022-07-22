
#include "buttons/buttons.h"




// static declarations
static void blank();

// global definitions
interrupt button0 = {BTN0, blank, GPIO_IRQ_EDGE_FALL};
interrupt button1 = {BTN1, blank, GPIO_IRQ_EDGE_FALL};
interrupt button2 = {BTN2, blank, GPIO_IRQ_EDGE_FALL};
interrupt button3 = {BTN3, blank, GPIO_IRQ_EDGE_FALL};


static void blank()
{

}
