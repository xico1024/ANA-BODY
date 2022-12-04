#ifndef Limites_h
#define Limites_h

#include "Arduino.h"
#include "Sonido.h"
#include "Pinout.h"

#define _INFO 1         //0 no 1 si

class Limites
{
  public:
    Limites(void);                         // constructor
    bool _HSW3anterior;
    int _HSW3Ant;
    byte sw1();                            // 0 abierto, 1 choco, 2 sigue chocado
    byte sw2();
    byte sw3();                            // limite antebrazo
    //bool anySW();                        // si choco algun sensor = 0
    long M1Inf(long currentM2);
    long M1Sup(long currentM2);
    long M2Inf(long currentM1);
    long M2Sup(long currentM1);
    int antebrazoSW();                     // Antebrazo, 0 = OK 1 = choco
    bool inside(long motor, long currentM1, long currentM2); //false si esta fuera de los limites o si toco algun sensor
    bool armConstrain (long ActualM1, long ActualM2, long incM1, long incM2, int info); //devuelve false si falla 

  private:
    Sonido sonido;
    byte _sw1Ant;
    byte _sw2Ant;
    byte _sw3Ant;
};
#endif
