// ================================================================
// ===                           shock                          ===
// ================================================================
#define externalVoltage
#define volPIN 6
#define posPIN 0
#define multiplayer 4

// dane poczatkowe
const double maxshock = 182.5, minshock = 132.5, rockarm = 67, height = 157;
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

double readAngle();
void ShockSetup()
{
  int loops = 50;
  offsetAngle = 0;
  for (int i = 0; i < loops; i++)
  {
    offsetAngle += readAngle();
  }
  offsetAngle /= loops;
  //offsetAngle = readAngle();
}

double readAngle()
{
  double maxVoltage;
  double aRead;
#ifdef externalVoltage
  maxVoltage = analogRead(volPIN) * multiplayer;
  //  Serial.print("max voltage: ");
  //  Serial.print(maxVoltage);
  //  Serial.print("\t");
  aRead = analogRead(posPIN);
  //  Serial.print("analog read: ");
  //  Serial.print(aRead);
  //  Serial.print("\t");
#elif
  //double maxVoltage = analogRead(volPIN) * multiplayer;
  maxVoltage = 1024;
  aRead = analogRead(posPIN);
#endif
  int loops = 2;
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
  double readAngle = 220//3.839724354388 //220st w radianach
                     * aRead / maxVoltage;
  return readAngle;
}
double shocklength()
{
  double angle = readAngle() - offsetAngle + startangle;
  angle *= 0.0174532925;//zamiana na radiany
  angle = cos(angle);

  double shockLength = b * angle + sqrt(b2 * sq(angle) + c4);
  shockLength /= 2;
  //  Serial.print("shock length: ");
  //  Serial.print(shockLength);
  //  Serial.print("\t");
  return shockLength;
}
