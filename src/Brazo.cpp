/*
  Brazo.h - Library for ANA
  Created by Julian Petrina 2020.
*/
#include "Brazo.h";

uint8_t *heapptr, *stackptr; // para la funcion de chequeo de meme

bool _mArmOff;
unsigned long _ArmLastTime;
unsigned long _tArmAux;


Brazo::Brazo(void)
// Constructor
{
  pinMode(ENABLE_MOT, OUTPUT); // activo LOW
  pinMode(DIR1, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(STEP2, OUTPUT);
  digitalWrite(ENABLE_MOT, HIGH); //desahabilito motores
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  // Acomodo la direccion al cableado
  stepper1.setPinsInverted(true, false, false);
  stepper2.setPinsInverted(true, false, false);

  _mArmOff = false;
}

void Brazo::AVset(int stepper, long aceleracion, long velocidad)
// Setea la acelercion y velocidad  del motor n
{ 
  if (stepper == 1)
  {
    stepper1.setMaxSpeed(velocidad);
    stepper1.setAcceleration(aceleracion);
  }
  if (stepper == 2)
  {
    stepper2.setMaxSpeed(velocidad);
    stepper2.setAcceleration(aceleracion);
  }
  
}

long Brazo::currentPos(int motor)
// Devuelve la posicion actual del motor n
{ 
  long result;
  if (motor == 1)
  {
    result = stepper1.currentPosition();
  }
  if (motor == 2)
  {
    result = stepper2.currentPosition();
  }

  return result;
}

void Brazo::setTask(long incX, long incY)
{

}

bool Brazo::mover(long incX, long incY, long resize, int modo, bool info)
// Modos de dibujo
{ // Modos 1,2,3,4,5,6,7,8,9 Resize 0 100% para escalar
  // Devuelve True si el motor se movio
  bool result = false;
  digitalWrite(ENABLE_MOT, LOW); // Habilito motores

  // Aceleracion y velocidad / hay que sacarlos de aca y hacerlos generales.
  long UincX, UincY;
  A1 = random(AVMIN, AVMAX);
  V1 = random(AVMIN, AVMAX);
  A2 = random(AVMIN, AVMAX);
  V2 = random(AVMIN, AVMAX);

  AVset(1, A1, V1);
  AVset(2, A2, V2);

  _pAnt1 = currentPos(1);
  _pAnt2 = currentPos(2);
  _pFinal1 = currentPos(1) + incX;
  _pFinal2 = currentPos(2) + incY;

  // reSize de los movimientos
  incX = (incX * resize) / 100;
  incY = (incY * resize) / 100;

  if (info == true)
  {

    Serial.println(" INFO MOVER");
    Serial.print("\t Modo: ");
    Serial.println(modo);
    Serial.print("\t AV1: ");
    Serial.print(A1);
    Serial.print(", ");
    Serial.print(V1);
    Serial.print(" AV2: ");
    Serial.print(A2);
    Serial.print(", ");
    Serial.println(V2);
    Serial.print("\t Tamaño: ");
    Serial.print(resize);
    Serial.print("\t Incrementos: ");
    Serial.print(incX);
    Serial.print(", ");
    Serial.println(incY);
    Serial.print("\t Vieja Pos: ");
    Serial.print(_pAnt1);
    Serial.print(", ");
    Serial.println(_pAnt2);
    Serial.print("\t Nueva Pos: ");
    Serial.print(_pAnt1 + incX);
    Serial.print(", ");
    Serial.println(_pAnt2 + incY);
  }

  if (modo == 1)  // solo el motor 1
  {                     
    _moverMotor(1, incX); // M1 Solamente
  }
  if (modo == 2)  // solo el motor 2
  {                      
    _moverMotor(2, incY); // M2 Solamente
  }
  if (modo == 3)  // Primero M1 luego M2
  {                      
    _moverMotor(1, incX); // M1 --> M2
    delay(100);
    _moverMotor(2, incY);
  }
  if (modo == 4)  // Primero M2 luego M1
  {                      
    _moverMotor(2, incY); // M2 --> M1
    delay(100);
    _moverMotor(1, incX);
  }
  if (modo == 5)  // simultaneo
  {  
    stepper1.setMaxSpeed(V1 / 4);
    stepper2.setMaxSpeed(V2 / 4);
    //Serial.print("Vel 1:"); Serial.println(V1);
    //Serial.print("Vel 2:"); Serial.println(V2);
  
    
    _positions[0] = currentPos(1) + incX;
    _positions[1] = currentPos(2) + incY;
    if (_getValidMode(_positions[0], _positions[1]) == 5)
    {
      steppers.moveTo(_positions);
      steppers.runSpeedToPosition(); // Blocks until all are in position
    }
    else
    {
      if (_getValidMode(_positions[0], _positions[1]) == 3)
      {
        _moverMotor(1, incX);
        delay(100);
        _moverMotor(2, incY);
      }
      if (_getValidMode(_positions[0], _positions[1]) == 4)
      {
        _moverMotor(2, incY);
        delay(100);
        _moverMotor(1, incX);
      }
    }

    if (info == true)
    {
      Serial.print("\t VelSimultanea: ");
      Serial.println((V1 + V2) / 4);
      Serial.println();
    }
  }
  if (modo == 6)  // M1-->M2 ////PINTANDO////
  { 

    long pasos = abs(incX) / (pow(2, random(3, 6))); // aca se define los pasos del sombreado
    // pasos = pasos + 1; // para que sea impar
    if (pasos == 0) pasos = 2;
    long Xfinal = currentPos(1) + incX;
    long Yfinal = currentPos(2) + incY;
    UincX = incX / pasos;
    UincY = incY / pasos;

    for (int a = 1; a <= pasos; a++)
    {

      if (limite.armConstrain(currentPos(1), currentPos(2), 0, incY, 0))
      {
        stepper2.move(incY);
        do
        {
        } while (stepper2.run() and limite.inside(2, currentPos(1), currentPos(2)));
        incY = -incY;
      }
      delay(100);

      if (limite.armConstrain(currentPos(1), currentPos(2), UincX, 0, 0))
      {
        stepper1.move(UincX);
        do
        {
        } while (stepper1.run() and limite.inside(1, currentPos(1), currentPos(2)));
      }
      delay(100);
    }
    // lo llevo al destino definido por la secuencia
    if ((currentPos(1) != Xfinal) or (currentPos(2) != Yfinal))
    {
      _moverAbsoluto(Xfinal, Yfinal);
      Serial.print("*");
    }
  }
  if (modo == 7)  // M2-->M1 ////PINTANDO//// 
  { 

    long Xfinal = currentPos(1) + incX;
    long Yfinal = currentPos(2) + incY;
    long pasos = abs(incY) / (pow(2, random(3, 6)));
    if (pasos == 0) pasos = 2;
    UincX = incX / pasos;
    UincY = incY / pasos;

    for (int a = 1; a <= pasos; a++)
    {

      if (limite.armConstrain(currentPos(1), currentPos(2), incX, 0, 0))
      {
        stepper1.move(incX);
        do
        {
        } while (stepper1.run() and limite.inside(1, currentPos(1), currentPos(2)));
        incX = -incX;
      }
      delay(100);
      if (limite.armConstrain(currentPos(1), currentPos(2), 0, UincY, 0))
      {
        stepper2.move(UincY);
        do
        {
        } while (stepper2.run() and limite.inside(2, currentPos(1), currentPos(2)));
      }
      delay(100);
    }
    // lo llevo al destino definido por la secuencia
    if ((currentPos(1) != Xfinal) or (currentPos(2) != Yfinal))
    {
      _moverAbsoluto(Xfinal, Yfinal);
      Serial.print("*");
    }
  }
  if (modo == 8)  // M1-->M2 ////CURVAS////
  { 

    int iteraciones = random(1, 10);
    int m = 10;
    if (random(2) == 0)
    {
      A1 = random(AVMIN, AVMAX);
      V1 = random(AVMIN, AVMAX);
      A2 = A1 * random(1, m);
      V2 = V1 * random(1, m);
    }
    else
    {
      A2 = random(AVMIN, AVMAX);
      V2 = random(AVMIN, AVMAX);
      A1 = A2 * random(1, m);
      V1 = V2 * random(1, m);
    }

    AVset(1, A1, V1);
    AVset(2, A2, V2);
    int cont1 = 0;
    int cont2 = 0;
    int cont = 0;

    // verifico que pueda moverme con ambos motores
    if (limite.armConstrain(currentPos(1), currentPos(2), incX, incY, 0))
    {
      stepper1.move(incX);
      stepper2.move(incY);

      do
      {
        if (cont2 < iteraciones)
        {
          stepper2.run();
          if (((stepper2.distanceToGo() == 0) or limite.antebrazoSW() > 0))
          {               // alcanzo destino o choco con el SW
            incY = -incY; // invierto el destino
            stepper2.move(incY);
            cont2++;
          }
        }
        if (cont1 < iteraciones)
        {
          stepper1.run();
          if (((stepper1.distanceToGo() == 0) or limite.antebrazoSW()) > 0)
          {
            incX = -incX; // invierto el destino
            stepper1.move(incX);
            cont1++;
          }
        }

      } while ((cont1 + cont2) < iteraciones * 2);
    }

    if (info == true)
    {
      Serial.print("\t ITERACION  ");
      Serial.println(cont);
    }
  }
  result = true;
  _mArmOff = true;
  _ArmLastTime = millis();
  delay(500);
  return result;
}

void Brazo::run() 
// VER EN QUE CLASE TIENE QUE ESTAR DIBUJAR ?
{
 // if (abs(stepper1.distanceToGo()>0) and limite.inside(1, currentPos(1), currentPos(2))) stepper1.run();
 // if (abs(stepper2.distanceToGo()>0) and limite.inside(2, currentPos(1), currentPos(2))) stepper2.run();

}

void Brazo::goHome()
// Estaciona el brazo suavemente
{
  Serial.println("Homing.... ");

  delay(1000);

  // Chequeo ambos Switches   1 = abiertos = ok /  0 = cerrados
  while (limite.sw1() == 0 or limite.sw2() == 0)
  {
    if (limite.sw1() == 0)
      _backwardstep1(); // muevo un paso atras el hombro
    if (limite.sw2() == 0)
      _backwardstep2(); // muevo un paso atras el codo
    delay(2);
  }
  delay (200);

  // Muevo hacia adelante hasta soltar el SW
  
  while (limite.sw1() > 0)
  {
    _forwardstep1();
    delay(5);
  }
  while (limite.sw2() > 0)
  {
    _forwardstep2();
    delay(5);
  }
  delay (200);
  // Muevo hacia adelante hasta tocar y soltar HSw3
 
 
  while (limite.sw3()  == 0)
  {
    _backwardstep1();
    delay(5);
  }
 
  while (limite.sw3() > 0)
  {
      _forwardstep1();
    delay(5);
  }
 
  //_setCurrentPos(1, 0); // configiro en estas pos el 0 M1
  //_setCurrentPos(2, 0); // configiro en estas pos el 0 M2
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);

  delay(1000);
  //_spanSearch();
}

void Brazo::navegar()
{
  AVset(1, 5000, 1000);
  AVset(2, 5000, 1000);

  while (limite.sw1() == 0)
  {
    stepper1.runToNewPosition(stepper1.currentPosition() + 30);
    stepper2.runToNewPosition(limite.M2Sup(stepper1.currentPosition()));
    stepper1.runToNewPosition(stepper1.currentPosition() + 30);
    stepper2.runToNewPosition(limite.M2Inf(stepper1.currentPosition()));
  }

  // void proximoColor(bool info);
  delay(500);
  while (limite.sw1() > 0)
  {
    _backwardstep1();
    delay(2);
  }
  delay(500);
  stepper2.runToNewPosition(limite.M2Sup(stepper1.currentPosition()));
  // Manota.proximoColor(true);
  delay(2000);

  while (limite.sw1() == 0)
  {
    stepper1.runToNewPosition(limite.M1Inf(stepper2.currentPosition()));
    stepper2.runToNewPosition(stepper2.currentPosition() - 20);
    stepper1.runToNewPosition(limite.M1Sup(stepper2.currentPosition()));
    stepper2.runToNewPosition(stepper2.currentPosition() - 20);
  }
}

void Brazo::cargarPintura(int color)
//Lleva el brazo a la zona de carga de pintura
{
   digitalWrite(ENABLE_MOT, LOW);
  Serial.println("CARGA");


  AVset(1, 4000, 500);
  AVset(2, 4000, 500);
  stepper1.move(800);
  stepper2.move(1000);

 
    do
    {
      stepper1.run();
      //Serial.println (limite.sw3());
    } while (limite.sw3() != 1  );
    //Serial.println(stepper1.currentPosition());
    Serial.println("ETAPA 1 ");

do
    {
      stepper2.run();
      //Serial.println (limite.sw3());
    } while (limite.sw3() != 1  );
    //Serial.println(stepper2.currentPosition());
    Serial.println("ETAPA 2 ");


  stepper1.move(700);
  stepper2.move(800);
  do
    {
      stepper1.run();
      //Serial.println (limite.sw3());
    } while (limite.sw1() != 1);
    Serial.println("ETAPA 3 ");
  
  do
    {
      stepper2.run();
      //Serial.println (limite.sw3());
    } while (limite.sw3() != 1);
    Serial.println("ETAPA 4 ");
   
   digitalWrite(ENABLE_MOT, HIGH);

  }

void Brazo::_checkMem()
// Chequea si no hay problemas de memoria
{
  // uint8_t * heapptr, * stackptr;                                                  // I declared these globally
  stackptr = (uint8_t *)malloc(4); // use stackptr temporarily
  heapptr = stackptr;              // save value of heap pointer
  free(stackptr);                  // free up the memory again (sets stackptr to 0)
  stackptr = (uint8_t *)(SP);      // save value of stack pointer
  Serial.print("heapptr: ");
  Serial.println((int)heapptr);
  Serial.print("stackptr: ");
  Serial.println((int)stackptr);
}

void Brazo::_setCurrentPos(int motor, long pos)
// Setea la posicion del motor seleccionado
{ 
  if (motor == 1)
  {
    stepper1.setCurrentPosition(pos);
    Serial.println( stepper1.currentPosition());
  }

  if (motor == 2)
  {
    stepper2.setCurrentPosition(pos);
    Serial.println( stepper2.currentPosition());
  }

}

void Brazo::_DesactivarMotores(unsigned long tiempo)
// Desactivan los motores luego de un t de no uso
{
  tiempo = tiempo * 1000;
  _tArmAux = millis() - _ArmLastTime;
  // Serial.print ("_mArmOff ") ; Serial.println (_mArmOff); delay (500);

  if ((_mArmOff == true) and (digitalRead(ENABLE_MOT) == LOW))
  {
    if (_tArmAux > tiempo )
    {
      _mArmOff = false;
      digitalWrite(ENABLE_MOT, HIGH); // desactivo los motores  luego de un tiempo en mseg
      Serial.println("Motores desactivados");

      // digitalWrite (ENABLE_MOT, HIGH );  //desactivo los motores  luego de un tiempo en mseg
      // Serial.print ("t_actual "); Serial.println (millis());
      // Serial.print ("ArmLastTime "); Serial.println (_ArmLastTime);
      // Serial.print ("Mills-ArmAux "); Serial.println (_tArmAux);
    }
  }
}

void Brazo::_moverMotor(long motor, long inc)  
// Mueve el motor si el armConstrain se lo permite
{ 
  stepper1.setMaxSpeed(V1 / 4);
  stepper2.setMaxSpeed(V2 / 4);
  digitalWrite(ENABLE_MOT, LOW); // Habilito motores
  if (motor == 1)
  {
    if (limite.armConstrain(currentPos(1), currentPos(2), inc, 0, 2))
    {
      _positions[0] = currentPos(1) + inc;
      _positions[1] = currentPos(2) + 0;
      steppers.moveTo(_positions);
      steppers.runSpeedToPosition(); // Blocks until all are in position
    }
 
  }
  if (motor == 2)
  {
    if (limite.armConstrain(currentPos(1), currentPos(2), 0, inc, 2))
    {
      
      _positions[0] = currentPos(1) + 0;
      _positions[1] = currentPos(2) + inc;
      steppers.moveTo(_positions);
      steppers.runSpeedToPosition(); // Blocks until all are in position 
    }
  }

}

int Brazo::_getValidMode(long posX, long posY)
// Modos validos de movimiento desde pos actual hacia posXY (absoluto) devuelve 0,3,4,5 
{
  long incX = posX - currentPos(1);
  long incY = posY - currentPos(2);
  int outVal = 0;

  if (limite.armConstrain(currentPos(1), currentPos(2), incX, 0, 0) and limite.armConstrain(posX, currentPos(2), 0, incY, 0))
  {
    outVal = 3;
  }
  if (limite.armConstrain(currentPos(1), currentPos(2), 0, incY, 0) and limite.armConstrain(currentPos(1), posY, incX, 0, 0))
  {
    outVal = outVal + 4;
  }
  // Serial.print("R"); Serial.print(outVal); Serial.print("-");
  if (outVal == 7)
  {
    outVal = 5;
  }
  return outVal;
}

void Brazo::_moverAbsoluto(long posX, long posY)
// Usado por ej en funcion para cerrar dibujos modos 3 y 4 
{ 
  long IncX = posX - currentPos(1);
  long IncY = posY - currentPos(2);
  int val = _getValidMode(posX, posY);

  if (val == 3 or val == 5)
  {
    _moverMotor(1, IncX);
    delay(100);
    _moverMotor(2, IncY);
  }
  else if (val == 4)
  {
    _moverMotor(2, IncY);
    delay(100);
    _moverMotor(1, IncX);
  }
}

void Brazo::_forwardstep1()
// mas un paso Motor Hombro
{ 
  digitalWrite(ENABLE_MOT, LOW); //habilito 
  digitalWrite(DIR1, LOW);
  digitalWrite(STEP1, HIGH);
  delay(1);
  digitalWrite(STEP1, LOW);
}
void Brazo::_backwardstep1()
// menos un paso Motor Hombro
{ 
  digitalWrite(ENABLE_MOT, LOW);
  digitalWrite(DIR1, HIGH);
  digitalWrite(STEP1, HIGH);
  delay(1);
  digitalWrite(STEP1, LOW);
}
void Brazo::_forwardstep2()
// Motor Codo
{
  digitalWrite(ENABLE_MOT, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(STEP2, HIGH);
  delay(1);
  digitalWrite(STEP2, LOW);
}
void Brazo::_backwardstep2()
// Motor Codo
{ 
  digitalWrite(ENABLE_MOT, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(STEP2, HIGH);
  delay(1);
  digitalWrite(STEP2, LOW);
}

void Brazo::_spanSearch()
{
  AVset(1, 8000, 300);
  AVset(2, 8000, 300);
  stepper1.move(1600);
  stepper2.move(1600);

  do
  {
    do
    {
      if (limite.sw1() == 0)
        for (int i = 0; i < 20; i++)
          stepper2.runSpeed();
    } while (limite.sw1()==0 and limite.sw3()==0);
    do
    {
      if (limite.sw1() == 0)
        for (int i = 0; i < 20; i++)
          stepper1.runSpeed();
    } while (limite.sw1() == 0 and !limite.sw3()==0);
  } while (limite.sw1() == 0);
  Serial.print("ETAPA1 ");
  Serial.print(stepper1.currentPosition());
  Serial.print("\t");
  Serial.print(stepper2.currentPosition());
  Serial.println("\t");
  delay(1000);

  // stepper1.move(-1500); stepper2.move(-1500);
  stepper1.move(-stepper1.currentPosition());
  stepper2.move(-stepper2.currentPosition());
  AVset(1, 8000, 300);
  AVset(2, 8000, 300);

  for (int i = 0; i < 10; i++)
  {
    stepper2.runSpeed(); // lo separo del SW (CODO)
    delay(20);
  }
  delay(1000);

  do
  {
    for (int i = 0; i < 10; i++)
      stepper2.runSpeed();
  } while (limite.sw3()==0);
  Serial.print("ETAPA2 ");
  Serial.print(stepper1.currentPosition());
  Serial.print("\t");
  Serial.print(stepper2.currentPosition());
  Serial.println("\t");
  delay(1000);
  do
  {
    do
    {
      for (int i = 0; i < 20; i++)
        stepper1.runSpeed();
    } while (limite.sw3() > 0);
    delay(20);
    do
    {
      for (int i = 0; i < 20; i++)
        stepper2.runSpeed();
    } while (limite.sw3() == 0);
    delay(20);

  } while (limite.sw1()==0 and limite.sw2()==0 and (abs(stepper2.distanceToGo()) > 0));
  Serial.print("ETAPA3 ");
  Serial.print(stepper1.currentPosition());
  Serial.print("\t");
  Serial.print(stepper2.currentPosition());
  Serial.println("\t");

  {
    for (int i = 0; i < 10; i++)
      stepper1.runSpeed();
  }
  while (limite.sw1() == 0 and (abs(stepper1.distanceToGo()) > 0))
    ;
  Serial.print("ETAPA4 ");
  Serial.print(stepper1.currentPosition());
  Serial.print("\t");
  Serial.print(stepper2.currentPosition());
  Serial.println("\t");

  Serial.print("POSMAX M1: ");
  Serial.println(stepper1.currentPosition());
  Serial.print("POSMAX M2: ");
  Serial.println(stepper2.currentPosition());
}


/*
  bool Brazo::_inLimites (int motor, long inc){                                                                         // true si la posicion esta dentro de los limites.
  bool out = true ;
  long newPos;

  if (motor == 1){
    newPos = currentPos(1) + inc;
    if ((newPos < limite.M1Inf(currentPos(2))) or (newPos > limite.M1Sup(currentPos(2))) ) out = false;
  }
   if (motor == 2){
    newPos = currentPos(2) + inc;
    if ((newPos < limite.M2Inf(currentPos(1))) or (newPos > limite.M2Sup(currentPos(1))) ) out = false;
  }
  }
*/