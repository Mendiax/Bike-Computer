#include "core0.h"

// static functions
static void setup(void);
static int loop(void);

void core0_launch_thread(void)
{
    setup();
    while (loop())
    {
    }
}

static void setup(void)
{
    
}

static int loop(void)
{

}