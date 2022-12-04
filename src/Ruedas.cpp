#include "Ruedas.h"

Ruedas::Ruedas(void)
{
    pinMode(ENABLE_MOTR, OUTPUT); // activo LOW
    pinMode(DIR3, OUTPUT);
    pinMode(STEP3, OUTPUT);
    pinMode(DIR4, OUTPUT);
    pinMode(STEP4, OUTPUT);
    digitalWrite(ENABLE_MOTR, HIGH);

    // Acomodo la direccion al cableado
    stepper1.setPinsInverted(false, false, false);
    stepper2.setPinsInverted(true, false, false);
}

long Ruedas::mayor(long a, long b)
{
    if (a > b) return a;
    else return b;
}

long Ruedas::currentPos(int motor)
{ // devuelve la posicion actual del motor n
  long result;
  if (motor == 3)
  {
    result = stepper1.currentPosition();
  }
  if (motor == 4)
  {
    result = stepper2.currentPosition();
  }
 
  return result;
}

void Ruedas::_setCurrentPos(int motor, long pos)
{ // Setea la posicion del motor n

  if (motor == 3)
  {
    stepper1.setCurrentPosition(pos);
  }

  if (motor == 4)
  {
    stepper2.setCurrentPosition(pos);
  }
}

void Ruedas::AVset(long a1, long v1, long a2, long v2)                              // Setea la acelercion y velocidad  
{ 
    stepper1.setMaxSpeed(v1);
    stepper1.setAcceleration(a1);
    stepper2.setMaxSpeed(v2);
    stepper2.setAcceleration(a2);
}

void Ruedas::MoverRuedas(long modo, long inc, long vel, long acel)
{
    digitalWrite(ENABLE_MOTR, LOW);
    AVset(acel, vel, acel, vel);

    if (modo == 1)  {stepper1.move(inc); stepper2.move(inc);}
    if (modo == 2)  {stepper1.move(-inc); stepper2.move(-inc);}
    if (modo == 3)  {stepper1.move(inc); stepper2.move(-inc);}
    if (modo == 4)  {stepper1.move(-inc); stepper2.move(inc);}
    do {stepper1.run(); stepper2.run(); } while (abs(stepper1.distanceToGo()) > 0);   // HACER RUN GLOBAL

    // _mWheelOff = true;
    // _WheelsLastTime = millis(); // ultimo momento de uso
    // digitalWrite (ENABLE_MOTR, HIGH );
}

void Ruedas::Giro(int dist, int tipo, int vel, int acel){
//tipo 1 giran las dos ruedas
//tipo 2 gira la derecha
//tipo 3 gira la izquierda
long inc1, inc2;
if (tipo == 1) {inc1 = dist; inc2 = -dist;}
if (tipo == 2) {inc1 = dist; inc2 = 0;}
if (tipo == 3) {inc1 = 0; inc2 = -dist;}

AVset(acel, vel, acel, vel);
 
for (int a = 0; a < (dist); a++)
  {
    stepper1.run();
    stepper2.run();
  }
}

void Ruedas::Bailar(int bassPower, int midPower, int trebPower, int bassNotes, int trebNotes, int pedal)
{
  int totalPwr;
  int dir = 1;
  int modo = 1;  //giro  recto
  int sent = 1; 
  int pwrMult = 1;
  digitalWrite(ENABLE_MOTR, LOW);
 /*
  Serial.println(bassPower);
  Serial.println(midPower);
  Serial.println(trebPower);
  Serial.println(bassNotes);
  Serial.println(trebNotes);
  Serial.println(pedal);
  */

  AVset(bassNotes * 500, bassPower * 20, bassNotes * 500,  midPower * 20);
 
  if (bassNotes % 2 == 0) dir = -1 * dir;  //para girar a un lado o al otro, adelante atras
  if (trebNotes % 2 == 0) modo = -1 * modo; 
  if (pedal == 127) sent = -1 * sent;
  if (bassNotes > 0) pwrMult = bassNotes + trebNotes; else pwrMult=1;
   
  if (modo == 1 ) totalPwr =  midPower;  // recto o Giro
  else totalPwr =  bassPower + midPower + trebPower;
  
  pwrMult = pwrMult * pwrMult;
  long inc1 = dir * sent * totalPwr  * pwrMult;
  long inc2 = dir * sent * totalPwr  * pwrMult * modo;

  stepper1.move(inc1);
  stepper2.move(inc2);

}

void Ruedas::motoresRun()
{
    if abs(stepper1.distanceToGo()>0) stepper1.run();
    if abs(stepper2.distanceToGo()>0) stepper2.run();
} 

