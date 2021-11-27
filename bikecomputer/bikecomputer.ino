//#define NDEBUG
//#define SIM_INPUT
#include <MemoryFree.h>
#include <Wire.h>
#define PCDEBUG

#include "sensors/rearshock.h"

//#include "GyroRead.h"

#include "display/display.h"
#define REFRESH_RATE_TARGET_HZ 30
#define MIN_DELAY (1000 / REFRESH_RATE_TARGET_HZ)

#include "sensors/data.h"

//#include "addons/timer.h"

//----------------------------------
#include "sensors/speedmeter.h"

//----------------------------------

#define PIN_BUTTONS A3
#include "sensors/buttons.h"

#include "addons/print.h"

//RingBuffer* rb = RING_BUFFER_CREATE(sizeof(byte), SCREEN_WIDTH);
uint8_t allocatedMemory_rearShockBuffer[sizeof(byte)*SCREEN_WIDTH + sizeof(RingBuffer)] = {0};
uint8_t allocatedMemory_speedBuffer[sizeof(byte)*SCREEN_WIDTH + sizeof(RingBuffer)] = {0};

SensorData sensorData =  (SensorData){
    .rearShockBuffer = ring_buffer_create_static(allocatedMemory_rearShockBuffer,sizeof(byte), SCREEN_WIDTH),
    .speedBuffer = ring_buffer_create_static(allocatedMemory_speedBuffer,sizeof(byte), SCREEN_WIDTH),
    .speedMax = 0.0,
    .speedAvgVal = 0.0,
    .speedAvgCnt = 0,
    .speedDistance = 0,
    .time = 0
    };

  
//RING_BUFFER_CREATE(sizeof(byte), SCREEN_WIDTH);
void setup()
{
  Wire.begin();
#ifndef NDEBUG
  Serial.begin(115200);
  DEBUG_PRINT(F("PCDEBUG is defined"));
#endif
  /*sensors*/
#ifdef gyro
  gyrosetup();
#endif
  Display_init(&sensorData);
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  sensorData.speedAvgCnt = 0.0;
  sensorData.speedAvgVal = 0.0;
  sensorData.speedMax = 0.0;
  rearShock_init();
  speed_new();

  //  qmc.init();
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
}

float velocity;
double shockUsage;
#ifdef SIM_INPUT
double time = 0.0;
#endif

bool isButtonPressed = false;
unsigned long start;
unsigned distance;
Buttons pressedButton = btnNone;
Buttons pressedButtonBuffer;
unsigned long stopTime = 0;
unsigned long stopTimeCurrent = 0;
void loop()
{
  //DEBUG_PRINT(" update loop ");
  start = millis();
  pressedButton = btnNone;
  velocity = speed_mps_to_kmph(speed_getSpeed());
  shockUsage = rearShock_getShockUsage();
  sensorData.speedDistance = speed_getDistance() / 1000;
  //Serial.println(shockUsage);
#ifdef SIM_INPUT
  velocity = (sin(time) + 1.0) * 15.0;
  time += 0.1;
  speed_wheelCounter++;
#endif
  sensorData.speedMax = max(sensorData.speedMax, velocity);
  //update avg speed
  if (velocity == 0)
  {
    /*sensorData.speedAvgCnt += 1.0;
    sensorData.speedAvgVal *= (sensorData.speedAvgCnt - 1.0) / sensorData.speedAvgCnt;
    sensorData.speedAvgVal += velocity / sensorData.speedAvgCnt;*/
  }
  sensorData.time = millis() / 1000;
  sensorData.speedAvgVal = speed_getDistance() / sensorData.time;
  do
  {
    // time for update data
    pressedButtonBuffer = getButton();
    if (pressedButtonBuffer != btnNone && isButtonPressed == false)
    {
      pressedButton = pressedButtonBuffer;
      isButtonPressed = true;
    }
    if (pressedButtonBuffer == btnNone)
    {
      isButtonPressed = false;
    }
    delay(10);
  } while (millis() - start < MIN_DELAY);

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
  byte bufferVelocity = (byte) velocity;
  byte bufferShock = (byte) shockUsage;

  ring_buffer_push_overwrite(sensorData.speedBuffer, (char *)&bufferVelocity);
  ring_buffer_push_overwrite(sensorData.rearShockBuffer, (char *)&bufferShock);

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
