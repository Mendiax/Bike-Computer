//#define NDEBUG
//#define SIM_INPUT

#include <Wire.h>
#define PCDEBUG

#include "sensors/rearshock.h"

//#include "GyroRead.h"

#include "display/display.h"
#define REFRESH_RATE_TARGET_HZ 10
#define MIN_DELAY (1000 / REFRESH_RATE_TARGET_HZ)

#include "sensors/data.h"

//#include "addons/timer.h"

//----------------------------------
#include "sensors/speedmeter.h"
#define PIN_SPEED 2
#define WHEEL_SIZE (0.6985 * PI)
//----------------------------------

#define PIN_BUTTONS A3
#include "sensors/buttons.h"

#include "addons/print.h"

SensorData sensorData = {0};

void setup()
{
  Wire.begin();
#ifndef NDEBUG
  Serial.begin(115200);
#endif
  DEBUG_PRINT("PCDEBUG is defined");

  /*sensors*/
#ifdef gyro
  gyrosetup();
#endif

  sensorData.velocity = ring_buffer_create(sizeof(double), SCREEN_WIDTH);
  speed_new(PIN_SPEED, WHEEL_SIZE);
  Display_init(&sensorData);
  Display_setDisplayType(DisplayType::lastVal);
  //ShockSetup();

  //  qmc.init();
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
  //Serial.print("MAX_TIME: ");
  //Serial.println(max_time);
}
//Timer displayTimer("DISPLAY UPDATE");

double velocity;
#ifdef SIM_INPUT
double time = 0.0;
#endif

bool isButtonPressed = false;
void loop()
{
  //DEBUG_PRINT(" update loop ");
  long start = millis();
  Buttons pressedButton = btnNone;
  Buttons pressedButtonBuffer;
  do
  {
    // time for update data
    pressedButtonBuffer = getButton();
    if (pressedButtonBuffer != btnNone && isButtonPressed == false)
    {
      pressedButton = pressedButtonBuffer;
      isButtonPressed = true;
    }
    if(pressedButtonBuffer == btnNone){
      isButtonPressed = false;
    }

    velocity = speed_mps_to_kmph(speed_getSpeed());
    //delay(100);
  } while (millis() - start < MIN_DELAY);
#ifdef SIM_INPUT
  velocity = (sin(time) + 1.0) * 15.0;
  time += 0.1;
#endif
  switch (pressedButton)
  {
  case btnLeft:
    Display_incDisplayType();
    break;
  case btnMiddle:
    Display_decDisplayType();
  default:
    break;
  }

  ring_buffer_push_overwrite(sensorData.velocity, (char *)&velocity);
  //printMem(sensorData.velocity->data_pointer, sensorData.velocity->size_of_element * sensorData.velocity->max_queue_length);
  //Serial.println(velocity);
  //DEBUG_PRINT("display update");
  //displayTimer.start();
  Display_update();
  //displayTimer.stop();
  //displayTimer.printLogSeconds();
  //DEBUG_PRINT("display update end");

  /*
#ifdef gyro
  gyroloop();
#endif*/
}
