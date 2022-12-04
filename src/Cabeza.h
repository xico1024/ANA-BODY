#ifndef Cabeza_h
#define Cabeza_h

#include "Arduino.h"
#include "Stream.h"
#include <Wire.h>

class Cabeza{
  public:
    Cabeza(void); //constructor
    void envioColorI2C(int red, int green, int blue) ;
    void envioModoI2C(int modo, int periodo, int fade);
 
    String color;
    String modo;
    bool cambio_color;                // Global para determinar si el color cambio
    bool cambio_modo;                 // Global para determinar si el modo cambio
    bool custom_msg = false;

  private:
    int RAnt, GAnt, BAnt;                    
    int r, g, b;                              // colores de la cabeza
    int m, p, f;                              // Modo, perido y fade  de la cabeza
    
    static void I2CrequestEvent(); 
       
};
#endif
