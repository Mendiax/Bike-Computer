#include "core1.h"
#include "types.h"
#include "buttons/buttons.h"
#include "traces.h"

// display
#include "display/print.h"
//veiws
#include "views/display.h"

#include <string>
#include "pico/time.h"

#define FRAME_PER_SECOND 10

// static variables
static SensorData sensorDataDisplay = {0};
static volatile uint32_t lastBtn1Press = 0;
static volatile uint32_t btnPressedCount = 0;

// static functions
static void setup(void);
static int loop(void);
static constexpr int64_t fpsToUs(uint8_t fps);

// global variables
SensorData sensorData = {0};
mutex_t sensorDataMutex;

// global functions
void core1LaunchThread(void)
{
    setup();
    while (loop())
    {
    }
}

//static functions definitions


//button
static void incDisplay(void)
{
    if(to_ms_since_boot(get_absolute_time()) - lastBtn1Press < 500)
    {
        return;
    }
    btnPressedCount++;
    btnPressedCount = to_ms_since_boot(get_absolute_time());
}
static void incDisplayRel(void)
{
    btnPressedCount = to_ms_since_boot(get_absolute_time());
}

template <typename T>
static constexpr int64_t fpsToUs(T fps)
{
    return (1000 * 1000)/(int64_t)fps;
}


static void setup(void)
{
    mutex_init(&sensorDataMutex);
    button1.callbackFunc = incDisplay;
    button1rel.callbackFunc = incDisplayRel;
    interruptSetupCore1();

    sensorDataDisplay = sensorData;
    
    Display_init(&sensorDataDisplay);
}

static int loop(void)
{
    // TODO time it and mak it refresh at given fps
    absolute_time_t frameStart = get_absolute_time();

    // frame update
    { 
        // copy data
        mutex_enter_blocking(&sensorDataMutex);
        sensorDataDisplay = sensorData;
        mutex_exit(&sensorDataMutex);

        if(btnPressedCount > 0)
        {
            Display_incDisplayType();
            btnPressedCount = 0;
        }

        // render
        Display_update();
        display::display();
    }

    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    if(fpsToUs(FRAME_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs; 
        sleep_us(timeToSleep);
        TRACE_DEBUG(1, TRACE_CORE_1, "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",frameTimeUs, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    }
    else
    {
        int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs; 
        TRACE_ABNORMAL(TRACE_CORE_1, "frame took %" PRIi64 " should be %" PRIi64 " delta %" PRIi64 "\n",frameTimeUs, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    }
    return 1;
}
