#ifndef Geoloc_h
#define Geoloc_h

#define echoPin 11 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10 //attach pin D3 Arduino to pin Trig of HC-SR04
#include "Arduino.h"


class Geoloc 
{
  public:
    Geoloc (void); //constructor
    int Medir();
   
private:
    long duration; // variable for the duration of sound wave travel
    int distance;  // variable for the distance measurement

                
};

#endif
