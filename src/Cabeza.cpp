#include "Cabeza.h"

Cabeza::Cabeza(void) { 
    //Wire.begin(8);                              //inicializo I2c Address 8
    //Wire.onRequest(I2CrequestEvent);   
       }                                                    //Constructor

void Cabeza::envioColorI2C(int red, int green, int blue) {
  color = String(red) + "," + String(green) + "," + String(blue);

}
void Cabeza::envioModoI2C(int mode, int periodo, int f) {
  modo = String(mode) + "," + String(periodo) + "," + String(f);

}
