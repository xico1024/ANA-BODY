#ifndef Dibujar_h
#define Dibujar_h

#include "Arduino.h"
#include "Secuencia.h"
#include "Manox3.h"
#include "Brazo.h"

class Dibujar : public Manox3 
{
public:
  Dibujar(void);                                              // constructor
  void loop ();                                                //ejecutarlo una vez por ciclo
  void pintar (long x2, long y2, long iteraciones, bool info); // figuras tipo lissajous
  void seqForma (long nTrazos, int mode);                     // Crea una secuencia random (dibujo)
  void dibujarPalabra (String Texto);                          // transfromar y dibuja palabras en ideogramas
  void dibujaTexto (String Texto);                             // sequencia a partir  un texto
  void runSeq (long tamano, int intensidad, int dibuja);       // ejecuta secuencia creada por una secuencia
  void posActual();                                           // deja en posM1 y posM2 la posicion de los motores.
  void zonaCarga ();                                           
  void parking (); 
  String exportSeq ();                                         // exporta los comandos y valores de la secuencia en un string sep por comas
  long posM1, posM2;
  bool nuevaSeq = false;

private:
  Secuencia secuencia;
  Manox3 manoColor; 
  Brazo brazo;

};
#endif
