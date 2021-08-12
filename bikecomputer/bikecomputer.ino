#include <Wire.h>
#define PCDEBUG
#include "ShockLength.h"

#ifdef gyro
#include "GyroRead.h"
#endif

#include "Screen.h"

//----------------------------------
#include "speedmeter.h"
#define PIN_SPEED 3
#define WHEEL_SIZE (2 * 0.6985 * PI)
//----------------------------------

#define REFRESH_RATE_TARGET_HZ 60
#define MIN_DELAY (1000 / 60)

#include "print.h"

float velocity;
enum displayType
{
  plot,
  lastVal
};

enum dataType
{
  speed,
  suspension,
  gyro
};

RingBuffer *mainBuffer;
displayType currentDisplayType = displayType::plot;
dataType currentDataType = dataType::speed;

void setup()
{
  Wire.begin();
#ifdef PCDEBUG
  Serial.begin(115200);
  printArgs("PCDEBUG is on\n");
  printArgs("%s is running on %d\n","Serial",115200);
#endif
  delay(100000);
#ifdef gyro
  gyrosetup();
#endif
  mainBuffer = ring_buffer_create(sizeof(double), SCREEN_WIDTH);
  speed_new(PIN_SPEED, WHEEL_SIZE);
  Screen_Setup();
  //ShockSetup();

  //  qmc.init();
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
  //Serial.print("MAX_TIME: ");
  //Serial.println(max_time);
}

void loop()
{
  long start = millis();
  do
  {
    // time for update data
    velocity = speed_getSpeed();
  } while (millis() - start < MIN_DELAY);
  //speed_update();
  //printOnPC(speed_velocity);

  double data;

  switch (currentDataType)
  {
  case dataType::speed:
    data = velocity;
    break;

  default:
    data = 0;
    break;
  }
  //update
  ring_buffer_push_overwrite(mainBuffer, (char *)&data);

  //display data on screen
  switch (currentDisplayType)
  {
  case displayType::plot:
    Screen_drawPlotRingBuffer(mainBuffer, 0, 20, double);
    break;

  default:
    break;
  }

  printOnPC(velocity);

  /*
#ifdef gyro
  gyroloop();
#endif*/
}
