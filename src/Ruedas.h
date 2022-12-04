#ifndef Ruedas_h
#define Ruedas_h

#include "Arduino.h"
#include <AccelStepper.h>
#include "Pinout.h"

#define MOTOR_TIMETOOFF 5000 // Cuando  se desenergizan luego de usarlos

class Ruedas
{
public:
    Ruedas(void); // constructor
    long mayor(long a, long b);
    long currentPos(int motor);
    void AVset(long a1, long v1, long a2, long v2);
    void MoverRuedas(long modo, long inc, long vel, long acel);
    void Giro(int dist, int tipo, int vel, int acel);
    void Bailar(int bassP, int midP, int trebP, int bassN, int TrebN, int pedal);
    void motoresRun();


private:
    AccelStepper stepper1 = AccelStepper(motorInterfaceType, STEP3, DIR3);
    AccelStepper stepper2 = AccelStepper(motorInterfaceType, STEP4, DIR4);
    void _setCurrentPos(int motor, long pos);
    unsigned long _WheelsLastTime;
};

#endif