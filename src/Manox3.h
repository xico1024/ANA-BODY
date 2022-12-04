/*
  Manox3V2.h - Library for ANA
  Created by Julian Petrina 2020.
  Maneja 3 colores mediante un motor y dos sensores
*/
#ifndef Manox3_h
#define Manox3_h

#include "Arduino.h"
#include <Servo.h>

// pines usados
#define _luzPin 45
#define _buzzerPin 27
#define _servoPin 23
#define _colorPin 62

class Manox3
{
public:
  Manox3(void);
  int proximoColor();
  int color();              // 1,2,3
  int pStatus();
  int pColor();
  int penStat;
  int penColor;
  void colorInit();
  void setColor(int color); // 1,2,3
  void lapizUp();
  void lapizDown();
  void luzOn(int intensidad);
  void luzOff();
  void buzzer(int tiempo);

private:
  Servo myservo;
};

#endif
