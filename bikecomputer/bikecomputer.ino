#include <Wire.h>
#include "ShockLength.h"
#include "GyroRead.h"
#ifndef Screen
#include "Screen.h"
#endif

#define pcDebug

/*print in serial in range 0-100*/
void printOnPC(double data)
{

  Serial.print(100);
  Serial.print("\t");
  Serial.print(0);
  Serial.print("\t");
  Serial.print(data);
  Serial.println("\t");

}

void setup() {
  Wire.begin();
#ifdef pcDebug
  Serial.begin(115200);
#endif
#ifdef gyro
  gyrosetup();
#endif

  ScreenSetup();
  ShockSetup();
  //  qmc.init();
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
}

double dataToDisplay[SCREEN_WIDTH] = {0};
byte Position = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int loops = 0;
int Frames = 0;
void loop() {
  travel_usage = 0;
  loops = 0;
  do
  {
    //Serial.print("do/while\t");
    currentMillis = millis();
    travel_usage += (maxshock - shocklength()) / travel * 100;
    loops++;
  } while (currentMillis - previousMillis < refreshTime);
  previousMillis = currentMillis;
  travel_usage /= loops;
#ifdef pcDebug
  printOnPC(travel_usage);
#endif
//  Travel[Position] = travel_usage;
  if (++Position == SCREEN_WIDTH)
    Position = 0;
  if (Frames >= frames)
  {
//    drawPlot(Travel, Position);
    Frames = 0;
  }
  else
    Frames++;

#ifdef gyro
  gyroloop();
#endif



}
