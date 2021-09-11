// ================================================================
// ===                           shock                          ===
// ================================================================
#define externalVoltage
#define volPIN 7
#define posPIN 6
#define multiplayer 3

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

double rearShock_readAngle(int loops);
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

static inline double rearShock_getVoltage()
{
  return analogRead(volPIN) * multiplayer;
}

/*get avg angle of x loops*/
double rearShock_readAngle(int loops)
{
  double maxVoltage;
  double aRead;
#ifdef externalVoltage
  maxVoltage = rearShock_getVoltage();
#elif
  maxVoltage = 1024;
#endif
  aRead = analogRead(posPIN);
  for (int i = 1; i < loops; i++)
  {
    maxVoltage += analogRead(volPIN) * multiplayer;
    aRead += analogRead(posPIN);
  }
  maxVoltage /= loops;
  aRead /= loops;

  //  Serial.print("read angle: ");
  //  Serial.print(readAngle);
  //  Serial.print("\t");
  double readAngle = 220 //3.839724354388 //220st w radianach
                     * aRead / maxVoltage;
  return readAngle;
}
double rearShock_getShocklength()
{
  double angle = rearShock_readAngle(20) - offsetAngle + startangle;
  angle *= 0.0174532925; //zamiana na radiany
  angle = cos(angle);

  double shockLength = b * angle + sqrt(b2 * sq(angle) + c4);
  shockLength /= 2;
  //  Serial.print("shock length: ");
  //  Serial.print(shockLength);
  //  Serial.print("\t");
  return shockLength;
}

double rearShock_getShockUsage(){
  return 100.0 - ((rearShock_getShocklength() - minshock)/travel) * 100.0;
}
