#include "Geoloc.h"
#define echoPin 11 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 10 //attach pin D3 Arduino to pin Trig of HC-SR04

Geoloc::Geoloc(void) {   
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT);  // Sets the echoPin as an INPUT
}   


int Geoloc::Medir() {

  digitalWrite(trigPin, LOW);        // Clears the trigPin condition
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);       // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  // Calculating the distance
  distance = duration * 0.034 / 2;   // Speed of sound wave divided by 2 (go and back)

  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");
  return distance;
}