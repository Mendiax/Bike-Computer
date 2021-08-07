#include <Wire.h>
#define PCDEBUG
#include "ShockLength.h"

#ifdef gyro
  #include "GyroRead.h"
#endif

#include "Screen.h"
#include "speedmeter.h"



/*print in serial in range 0-100*/
void printOnPC(double data)
{

  //Serial.print(100);
  //Serial.print("\t");
  Serial.print(0);
  Serial.print("\t");
  Serial.print(data);
  Serial.println("\t");

}

void setup() {
  Wire.begin();
#ifdef PCDEBUG
  Serial.begin(115200);
#endif
#ifdef gyro
  gyrosetup();
#endif

  speed_setup(3,SCREEN_WIDTH);
  ScreenSetup();
  //ShockSetup();

  //  qmc.init();
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
  //Serial.print("MAX_TIME: ");
  //Serial.println(max_time);
}

double dataToDisplay[SCREEN_WIDTH] = {0};
byte Position = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int loops = 0;
int Frames = 0;


float v,prev_v;

#define LOOP_FOR(delay_millis, ...) ({
  long start = millis();
  do{...}while(millis() - start < delay_millis)})

void loop() {
  LOOP_FOR(200,
  millis();
  //update data
  );
  //speed_update();
  //printOnPC(speed_velocity);
  v = speed_get_speed();
  ring_buffer_push_overwrite(speed_buffer, (char *)&prev_v);
  drawPlotRingBuffer(speed_buffer,0,20,float);
  printOnPC(prev_v);
  
/*  travel_usage = 0;
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
#ifdef PCDEBUG
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
#endif*/



}
