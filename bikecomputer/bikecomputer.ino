#define NDEBUG
#include <Wire.h>
#define PCDEBUG
#include "sensors/rearshock.h"

#ifdef gyro
#include "GyroRead.h"
#endif

#include "display/display.h"
#include "sensors/data.h"

//----------------------------------
#include "sensors/speedmeter.h"
#define PIN_SPEED 3
#define WHEEL_SIZE (2 * 0.6985 * PI)
//----------------------------------

#define REFRESH_RATE_TARGET_HZ 10
#define MIN_DELAY (1000 / 60)

#include "addons/print.h"

float velocity;

enum dataType
{
  speed,
  suspension,
  gyro
};

RingBuffer *mainBuffer;
dataType currentDataType = dataType::speed;

SensorData sensorData = {0};

void setup()
{
  Wire.begin();
#ifdef PCDEBUG
  Serial.begin(115200);
  //printArgs("PCDEBUG is on\n");
  //printArgs("%s is running on %d\n","Serial",115200);
#endif
  DEBUG_PRINT("ERROR TEST");
#ifdef gyro
  gyrosetup();
#endif
  sensorData.velocity = ring_buffer_create(sizeof(double), SCREEN_WIDTH);
  speed_new(PIN_SPEED, WHEEL_SIZE);
  Display_init(&sensorData);
  //ShockSetup();

  //  qmc.init();
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
  //Serial.print("MAX_TIME: ");
  //Serial.println(max_time);
}

void loop()
{
  DEBUG_PRINT(" update loop ");
  long start = millis();
  do
  {
    // time for update data
    velocity = speed_getSpeed();
  } while (millis() - start < MIN_DELAY);
  ring_buffer_push_overwrite(sensorData.velocity, (char*) &velocity);
  //Serial.println(velocity);
  DEBUG_PRINT("display update");
  Display_update();
  DEBUG_PRINT("display update end");

  /*
#ifdef gyro
  gyroloop();
#endif*/
}
