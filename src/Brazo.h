#ifndef Brazo_h
#define Brazo_h

#include "Arduino.h"
#include "Limites.h"
#include <AccelStepper.h>
#include <MultiStepper.h>

#define MOTOR_TIMETOOFF 5000 // Cuando  se desenergizan luego de usarlos
#define AVMAX 1200            //Maxima Velocidad
#define AVMIN 400             //Minima Vleocidad

class Brazo
{
public:
  Brazo(void); 
  void goHome();
  void AVset(int stepper, long aceleracion, long velocidad);
  void setTask(long incX, long incY);
  bool mover(long incX, long incY, long resize, int modo, bool info);
  long currentPos(int motor);
  void run();   
  void navegar();
  void cargarPintura(int color) ;     // Cargar pintura en el pincel   
 
  long A1, V1 ;
  long A2, V2 ;

private:
  long _pAnt1, _pAnt2;                // Posicion1 anterior luego de un movimiento
  long _pFinal1, _pFinal2;            // Posicion1 luego de aplicar incrementos
  long _positions[2];                 // Array para el multistepper
  
  void _spanSearch();
  bool _inLimites(int motor, long inc);
  int  _getValidMode(long posX, long posY); // Modos validos (0,3,4,5) de mov desde pos actual a posXY(absoluto)
  void _moverMotor(long motor, long inc);
  void _moverAbsoluto(long posX, long posY);
  void _checkMem();
  void _DesactivarMotores(unsigned long tiempo);
  void _setCurrentPos(int motor, long pos);
  
  // acceso a bajo nivel de los pasos del motor
  void _forwardstep1();
  void _backwardstep1();
  void _forwardstep2();
  void _backwardstep2();

  AccelStepper stepper1 = AccelStepper(motorInterfaceType, STEP1, DIR1);
  AccelStepper stepper2 = AccelStepper(motorInterfaceType, STEP2, DIR2);
  MultiStepper steppers;
  Limites limite;
};

#endif
