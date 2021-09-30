#ifndef __SHOCK_H__
#define __SHOCK_H__

#define posPIN 0

// dane poczatkowe
const double maxshock = 185.0, minshock = 132.5, rockarm = 67, height = 157;
double travel = maxshock - minshock;
double a = (sq(height) - sq(rockarm) - sq(maxshock)) / (-2 * rockarm * maxshock);
double startangle = (acos(a) * 57.2957795);

//do liczenia dlugosci dampera
double c4 = 4 * (sq(height) - sq(67));
double b = 2 * rockarm;
double b2 = sq(b);

//inne zmienne potrzebane
float startTime = 0;
double offsetAngle; // wartosc poczatkowa kata
double travel_usage;

double rearShock_readAngle(size_t loops);
void rearShock_init()
{
  int loops = 50;
  offsetAngle = 0;
  for (int i = 0; i < loops; i++)
  {
    offsetAngle += rearShock_readAngle(2);
  }
  offsetAngle /= loops;
  //offsetAngle = readAngle();
}

#define k 1000

//#define R_0 = 2.6
//#define R_1 = 37.7 * k
//#define R_2 = 114.4 * k
#define R_MAX (465.0 * k)
#define R3 (33.0 * k)

#define U_MAX 5.0

double rearShock_getResistance(double U1)
{
  double U0 = (U_MAX / U1 - 1.0);
  double a = U0;
  double b = -1.0 * (R3 + U0 * (R3 + R_MAX));
  double c = R3 * R_MAX;

  double delta = b * b - 4.0 * a * c;

  if (delta < 0.0){
        return -1.0;
  }
  double x1 = (-1.0 * b - sqrt(delta)) / (2 * a);
  double x2 = (-1.0 * b + sqrt(delta)) / (2 * a);
  double ret = min(x1, x2);
  Serial.print(ret);
  return ret;
}

/*get avg angle of x loops*/
double rearShock_readAngle(size_t loops)
{
  double aRead = analogRead(posPIN);
  
  for (size_t i = 1; i < loops; i++)
  {
    aRead += analogRead(posPIN);
  }
  aRead /= loops;

  double res = rearShock_getResistance((aRead * 5.0) / 1024.0) / R_MAX;
  double readAngle = 220.0 * res ;

  return readAngle;
}
double rearShock_getShocklength()
{
  double angle = rearShock_readAngle(20) - offsetAngle + startangle;
  angle *= 0.0174532925; //zamiana na radiany
  angle = cos(angle);

  double shockLength = b * angle + sqrt(b2 * sq(angle) + c4);
  shockLength /= 2;
  return shockLength;
}

double rearShock_getShockUsage()
{
  return 100.0 - ((rearShock_getShocklength() - minshock) / travel) * 100.0;
}

#endif
