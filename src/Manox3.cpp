/*
  Manox3.cpp - Library for ANA
  Created by Julian Petrina 2020.
  Funciona con un Servo modificaco y un switch que se cierra con cada color
*/

#include "Manox3.h"

Manox3::Manox3(void) 
{
  pinMode(_luzPin, OUTPUT);
  pinMode(_buzzerPin, OUTPUT);
  analogWrite(_luzPin, 0);
  pinMode(_colorPin, INPUT_PULLUP);
  //penStat=0;
}

void Manox3::buzzer(int tiempo)
{
  digitalWrite(_buzzerPin, HIGH);
  delay(tiempo);
  digitalWrite(_buzzerPin, LOW);
}

void Manox3::luzOn(int intensidad)
{
  digitalWrite(_luzPin, HIGH);
  /*
      for(int i=0; i < 255; i++){
      analogWrite(_luzPin, i);
      delay(10);
     }
     _brillo = 255;

  */
}

void Manox3::luzOff()
{
  digitalWrite(_luzPin, LOW);
  /*
      for(int i = _brillo; i>0; i--){
      analogWrite(_luzPin, i);
      delay(10);
      }
     _brillo = 0;
  */
}

int Manox3::proximoColor()
{ // devuelve el tiempo que tomo cambiar de color, con eso se determina cual es el primero
  // Serial.println();
  // Serial.print("PROX COLOR ");
  long aux = millis();
  long taux = 0;
  myservo.attach(_servoPin);
  myservo.write(180);
  delay(300);
  do
  {
    taux++;
  } while ((digitalRead(_colorPin) == 1) and (taux < 500000)); // tiempo para
  if (taux >= 500000)
  { // si hay problema y no ggira
    buzzer(1000);
    Serial.println("PROBLEMA SERVO !!");
    myservo.detach();
  }
  aux = millis() - aux;

  myservo.write(0);
  //delay(10);
  myservo.detach();

  penColor++;
  if (penColor > 3) penColor = 1;

  // Serial.println(aux);
  return aux;
}

void Manox3::colorInit()
{
  buzzer(100);
  do
  {
  } while (proximoColor() < 600); // los recorridos 1 y 2 son mas cortos
  penStat = 0;
  penColor = 1;
  
}

void Manox3::lapizDown()
{
  if (penStat == 0)
  {
    myservo.attach(_servoPin);
    myservo.write(0);
    delay(600); // presion
    myservo.detach();
    //myservo.attach(_servoPin);
    penStat=1;
  }
  delay(500);
}

void Manox3::lapizUp()
{
  if (penStat == 1)
  {
    myservo.attach(_servoPin);
    myservo.write(180);
    delay(150); // minimo para separar
    //  do {} while ( digitalRead(_colorPin) == 1 ); //freno cuando se cierra el sensor
    myservo.detach();
    penStat=0;
  }
  delay(500);
}

void Manox3::setColor(int c)
{
  if ((c > 0) and (c < 4) and (c != penColor))
  {
    if (penStat == 1)
    {
      lapizUp();
      do
      {
        proximoColor();
      } while (penColor != c);
      lapizDown();
    }
    else
    {
      do
      {
        proximoColor();
      } while (penColor != c);
    }
  }
}

int Manox3::pStatus() {return penStat;}

int Manox3::pColor() {return penColor;}