#include "core1.h"
#include "core_utils.hpp"
#include "common_types.h"
#include "buttons/buttons.h"
#include "traces.h"

// display
#include "display/print.h"
//veiws
#include "views/display.h"

#include <string>
#include "pico/time.h"

#define FRAME_PER_SECOND 10
#define MINIMAL_TIME_BTN 300

// static variables
static SensorData sensorDataDisplay = {0};
static volatile uint32_t lastBtn1Press = 0;
static volatile uint32_t btnPressedCount = 0;

static volatile uint32_t last_btn2_press = 0;
static volatile bool change_state = 0;


// static functions
static void incDisplay(void);
static void incDisplayRel(void);

static void change_state_irq_handler();
static void setup(void);
static int loop(void);

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
    if(to_ms_since_boot(get_absolute_time()) - lastBtn1Press < MINIMAL_TIME_BTN)
    {
        return;
    }
    btnPressedCount++;
    lastBtn1Press = to_ms_since_boot(get_absolute_time());
}
static void incDisplayRel(void)
{
    lastBtn1Press = to_ms_since_boot(get_absolute_time());
}

static void change_state_irq_handler()
{
    if(to_ms_since_boot(get_absolute_time()) - last_btn2_press < MINIMAL_TIME_BTN)
    {
        return;
    }
    change_state = true;
    last_btn2_press = to_ms_since_boot(get_absolute_time());
}


static void setup(void)
{
    mutex_init(&sensorDataMutex);
    button1.callbackFunc = incDisplay;
    button1rel.callbackFunc = incDisplayRel;

    button2.callbackFunc = change_state_irq_handler;
    button0.callbackFunc = change_state_irq_handler;
    button3.callbackFunc = change_state_irq_handler;

    interruptSetupCore1();

    sensorDataDisplay = sensorData;
    
    Display_init(&sensorDataDisplay);
}

static int loop(void)
{
    absolute_time_t frameStart = get_absolute_time();

    // frame update
    { 
        // copy data
        mutex_enter_blocking(&sensorDataMutex);
        sensorDataDisplay = sensorData;
        mutex_exit(&sensorDataMutex);

        // if system state has changed execute proper code
        static SystemState last_system_state;
        if(sensorDataDisplay.current_state != last_system_state)
        {
            last_system_state = sensorDataDisplay.current_state;
            switch(sensorDataDisplay.current_state)
            {
                case SystemState::TURNED_ON:
                    Display_set_main_display_type();
                    break;
                case SystemState::CHARGING:
                    Display_set_charge_display_type();
                    break;
                case SystemState::STOPPED:
                    // TODO ??
                case SystemState::PAUSED:
                case SystemState::RUNNING:
                default:
                    break;
            }
        }

        // if button has been pressed change view
        if(btnPressedCount > 0)
        {
            Display_incDisplayType();
            btnPressedCount = 0;
        }

        // if pause btn was pressed change state
        if(change_state)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "pause btn pressed %d\n", change_state);
            change_state = false;
            switch(sensorDataDisplay.current_state)
            {
                case SystemState::PAUSED:
                    sensorDataDisplay.current_state = SystemState::RUNNING;
                    break;
                case SystemState::RUNNING:
                    sensorDataDisplay.current_state = SystemState::PAUSED;
                    break;
                default:
                    TRACE_DEBUG(2, TRACE_CORE_1, "pause has no effect %d\n", change_state);
                    break;
            }
            mutex_enter_blocking(&sensorDataMutex);
            sensorData.current_state = sensorDataDisplay.current_state;
            mutex_exit(&sensorDataMutex);
        }

        // render
        Display_update();
        if(sensorDataDisplay.current_state == SystemState::PAUSED)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "printing pause label %d\n", change_state);
            Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
            const sFONT* font = 0;
            uint8_t scale;
            auto label = "PAUSED";
            auto width_char = pause_label.width / strlen(label); 
            getFontSize(width_char, pause_label.height, &font, &scale);
            Paint_Println(pause_label.x, pause_label.y, label, font, display::DisplayColor(0xf,0x0,0x0), scale);
        }
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
