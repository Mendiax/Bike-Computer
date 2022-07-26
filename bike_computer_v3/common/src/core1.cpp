#include "core1.h"
#include "types.h"
#include "buttons/buttons.h"

// display
#include "display/print.h"
//veiws
#include "views/display.h"

#include <string>

static void setup(void);
static int loop(void);

void core1LaunchThread(void)
{
    setup();
    while (loop())
    {
    }
}
SensorData sensorData = {0};
SensorData sensorDataDisplay = {0};
mutex_t sensorDataMutex;
volatile uint8_t btnPressedCount = 0;

void incDisplay(void)
{
    btnPressedCount++;
}

static void setup(void)
{
    mutex_init(&sensorDataMutex);
    button1.callbackFunc = incDisplay;
    interruptSetupCore1();

    sensorDataDisplay = sensorData;
    
    Display_init(&sensorDataDisplay);
}
static int loop(void)
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
        //getBatteryLevel();
        //std::string log = "bat:" + std::to_string(sensorDataDisplay.lipoLevel) + "\%";
        //Paint_Println( 0, 0, log.c_str(), &Font8);
        
        //std::string log = "btn:" + std::to_string(gpio_get(BTN));
        //Paint_Println(0 * Font8.width, 1 * Font8.height, log.c_str(), &Font8, 0x0f, 0x00);
        display::display();
        // add delay with fps cap
        return 1;
}
