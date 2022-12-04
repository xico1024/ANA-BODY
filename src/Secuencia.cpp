#include "Secuencia.h"

Secuencia::Secuencia(void) {}

long Secuencia::_randomGen (int motor, long M1, long M2) {                          //Genera  numero random (_limiteInf, _limiteSup)  MOTOR1
  long vAux;
  if (motor == 1)
  {
    _limiteSup =  limite.M1Sup(M2) - M1;
    _limiteInf =  - (M1 - limite.M1Inf(M2));
    vAux = random (_limiteInf, _limiteSup) ;
  }
  if (motor == 2)
  {
    _limiteSup =  limite.M2Sup(M1) - M2;
    _limiteInf =  - (M2 - limite.M2Inf(M1));
    vAux = random (_limiteInf, _limiteSup) ;
  }
  return vAux;
}

long Secuencia::_maxMotor(int motor, long inc, long actualM1, long actualM2) 
{ 
  long maximo;
  if (motor == 1)
  {
    if (inc >= 0) maximo =  limite.M1Sup(actualM2) - actualM1;
    else  maximo  =  - (actualM1 - limite.M1Inf(actualM2));  
  }

  if (motor == 2)
  {
    if (inc >= 0) maximo =  limite.M2Sup(actualM1) - actualM2;
    else  maximo  =  - (actualM2 - limite.M2Inf(actualM1));
   
  }
  return maximo;  
}

bool Secuencia::_validarSecuencia (bool info) {                        //TRUE si esta OK
  long val1 = _posOrigen1;
  long val2 = _posOrigen2;
  bool result = true;

  for (int i = 0; i < largoSeq; i++) {
    val1 = val1 + M1[i];
    val2 = val2 + M2[i];
    if (info){
    Serial.print("\n");
    Serial.print("\t [");
    Serial.print(val1); Serial.print("\t");
    Serial.print(val2); Serial.print("\t");
    Serial.print(i);
    Serial.print("]");
    }
    if (limite.armConstrain (val1, val2, M1[i], M2[i], 0) == false) {
      if (info) Serial.print (" *"); 
      result = false;
    }
  }
  if (info) Serial.println();
  return result;
}

bool Secuencia::_repararSecuencia (int _modo) {                         // Rota el dibujo y valida si esta OK  0,1,2,3 (0 no hace nada) TRUE si se reparo
  long M1aux [MAX_SEQ];                                               
  long M2aux [MAX_SEQ];                                               
  long aux;
  bool result;
  for (int i = 0; i < largoSeq; i++) {                               //salvo los valores en una tabla auxiliar
    M1aux[i] = M1[i];
    M2aux[i] = M2[i];
  }

  //Serial.println(_modo);
  if (_modo == 1) {
    for (int i = 0; i < largoSeq; i++) {
      M1[i] = -M1[i];
    }
  }

  if (_modo == 2) {
    for (int i = 0; i < largoSeq; i++) {
      M2[i] = -M2[i];
    }
  }

  if (_modo == 3) {
    for (int i = 0; i < largoSeq; i++) {
      aux = M1[i];
      M1[i] = M2[i];
      M2[i] = aux;
    }
  }

  result = _validarSecuencia (false);
  if (result == false) {
    for (int i = 0; i < largoSeq; i++) {                             //restituyo los valores.
      M1[i] = M1aux[i];
      M2[i] = M2aux[i];
    }
  }
  return result;
}

void Secuencia::_procesarLetrasXY (String Texto, int orden) {           //Arma secuencia M[] segun reglas de vocales y consonantes
  int largo = Texto.length();
  char letra ;
 // long dir1, dir2;
  //Serial.println(Texto);
  if (largo >= 2) { // Palabra corta

    letra = Texto.charAt(0);                  // el char devuelve el ASCII del primer caracter
    _incX = letra;                             // Lo asigno a un long
    if ( (letra % 2) == 0) _incX = -_incX;      // Cambia el signo si es par
    
    letra = Texto.charAt(1);                  // el char devuelve el ASCII
    _incY = letra;                             // Lo asigno a un long
    if ( (letra % 2) == 0) _incY = -_incY;
    
    M1[orden] = _incX;                          // inc M1
    M2[orden] = _incY;                          // inc M2  
    
    letra = Texto.charAt(0);                  // Segundo caracter
    Modo[orden] = 5;                          // consonantes _modo 5  
    if (letra == 'a' or letra == 'A') Modo[orden] = 1;   
    if (letra == 'e' or letra == 'E') Modo[orden] = 2;
    if (letra == 'i' or letra == 'I') Modo[orden] = 3;
    if (letra == 'o' or letra == 'O') Modo[orden] = 4;    
  }
}

void Secuencia::setOrigen (long pIni1, long pIni2) 
{
  _posOrigen1 = pIni1;                                // _incX inicial de la secuencia
  _posOrigen2 = pIni2;
}

void Secuencia::ArmarSecuencia (long nTrazos, int mode, bool info) 
//Armo la secuencia  _modo 0 = rnd, o fuerza _modo de mov 1 2 3 4 5
{
  int cont = 0;  
  _cerrada = 0;
  _taskIndex = 0;
  posInicial1 = _posOrigen1;
  posInicial2 = _posOrigen2;
  posActual1 = _posOrigen1;
  posActual2 = _posOrigen2;
   
   _cerrada = random(0, 2);       // determina si la secuencia vuelve a su punto inicial.
  if (mode > 0) _cerrada = 0;     // si es un _modo prefijado no es _cerrada
  Serial.println();
  Serial.print("GENERANDO SECUENCIA: ");
  if (_cerrada == 0) Serial.println ("ABIERTA");
  if (_cerrada == 1) Serial.println ("CERRADA");
  Serial.println ("\t [_incX  _incY MODO]"); 
  do {
    /*
    Loop N trazos
      _modos
      1- M1
      2- M2
      3- M1-->M2
      4- M2-->M1
      5- Movimientos simultaneos (M1 y M2)
      6 y 7 - Rayados (M1 y M2)
      n- Pintar entre (x1,y1) y (x2,y2) tipo lissajous
    */
    mode = 0; //////////FUERZO
    if (mode == 0) _modo = random(_minMode, (_maxMode + 1)); else _modo = mode;  //+1 es por el excluyente del Random

   
    posInicial1 = posActual1;
    posInicial2 = posActual2;
   
    if (_modo == 1) // Linea M1
    {
      _incX = _randomGen(1,posInicial1, posInicial2);  
      _incY = 0; 
    }

    if (_modo == 2) // Linea M2
    {
      _incY = _randomGen(2,posInicial1, posInicial2);  
      _incX = 0;   
    }
   
    if (_modo == 3) // Linea M1-->M2
    
    {
      _incX = _randomGen(1,posInicial1, posInicial2);
      _incY = _randomGen(2,posActual1 + _incX, posInicial2);
    }
   
    if (_modo == 4) // Linea M2-->M1
    {
      _incY = _randomGen(2,posInicial1, posInicial2); 
      _incX = _randomGen(1,posInicial1, posActual2 + _incY); 
    }
   
    if (_modo == 5) // Linea M1, M2 Simultaneo
    {

      if ((limite.M1Sup(posActual2) - limite.M1Inf(posActual2)) >= (limite.M2Sup(posActual1) - limite.M2Inf(posActual1))) {  //veo quien se puede mover mas M1 o M2
        _incX = _randomGen(1,posInicial1, posInicial2);
        _incY = _randomGen(2,posActual1 + _incX, posInicial2);
      }
      else {
        _incY = _randomGen(2,posInicial1, posInicial2);
        _incX = _randomGen(1,posInicial1, posActual2 + _incY);
      }
    }

    if (_modo == 6) // RAYADO M1-->M2
    {
      _incX = _randomGen(1,posInicial1, posInicial2);
      _incY = _randomGen(2,posActual1 + _incX, posInicial2);
      long maxM2 = _maxMotor (2, _incY, posInicial1, posInicial2);
      if (abs(_incY) > maxM2) _incY = maxM2;
    }
   
    if (_modo == 7) // RAYADO M1-->M2
    {
      _incX = _randomGen(1,posInicial1, posInicial2);
      _incY = _randomGen(2,posActual1, posInicial2 + _incY);
      long maxM1 = _maxMotor (1, _incX, posInicial1, posInicial2);
      if (abs(_incX) > maxM1) _incX = maxM1;
    }

    if (_modo == 8) // Pintar entre (x1,y1) y (x2,y2) tipo lissajous
    {

      if ((limite.M1Sup(posActual2) - limite.M1Inf(posActual2)) >= (limite.M2Sup(posActual1) - limite.M2Inf(posActual1))) {  //veo quien se puede mover mas M1 o M2
        _incX = _randomGen(1,posInicial1, posInicial2);
        _incY = _randomGen(2,posActual1 + _incX, posInicial2);
      }
      else {
        _incY = _randomGen(2,posInicial1, posInicial2);
        _incX = _randomGen(1,posInicial1, posActual2 + _incY);
      }
    }

    M1[cont] = _incX;                                               // inc M1
    M2[cont] = _incY;                                               // inc M2
    Modo[cont] = _modo;                                              // _modo
    SeqCerrada = _cerrada;                                          // SeqCerrada o abierta
    
    if (_modo == 1 or _modo == 2 or _modo == 5) addTask (_incX, _incY, 5);
    if (_modo == 3) { addTask (_incX, 0, 5); addTask (0, _incY, 5);}
    if (_modo == 4) { addTask (0, _incY, 5); addTask (_incX, 0, 5);}
    if (_modo > 5) addTask (_incX, _incY, _modo);
   
    valoracionInt[cont] = 0;                                        // Para las emociones
    animo[cont] = 0;                                                // Para las emociones
    valoracionExt[cont] = 0;                                        // Para las emociones
    animoExt[cont] = 0;                                             // Para las emociones
 
    posActual1 = posInicial1 + _incX;
    posActual2 = posInicial2 + _incY;
    cont = cont + 1;
  } while (cont < nTrazos);

  if (info) 
  {
    for (int i = 0; i < nTrazos; i++) 
    {
      Serial.print("\t [");
      Serial.print( M1[i]); Serial.print("\t");
      Serial.print( M2[i]); Serial.print("\t");
      Serial.print( Modo[i]);
      Serial.println("]");
    }
    Serial.println("\t ------------------");         
   // matrixLenght = _taskIndex;
    for (int i = 0; i < matrixLenght; i++) 
    {
      Serial.print("\t [");
      Serial.print(matrixM1[i]); Serial.print("\t");
      Serial.print(matrixM2[i]); Serial.print("\t");
      Serial.print(matrixModo[i]); Serial.println("]");
    }
    Serial.println("\t ==================");
    _incTotalX = posActual1 - _posOrigen1;
    _incTotalY = posActual2 - _posOrigen2;
    Serial.print("\t ["); Serial.print (_incTotalX); Serial.print ("\t"); Serial.print (_incTotalY); Serial.println("] INC TOTAL");
  }
  posActual1 = _posOrigen1;
  posActual2 = _posOrigen2;
  largoSeq = nTrazos; 
}

String Secuencia::proximaPalabra(String Texto) {                       //Parsea un texto y devuelve de a una las palabras,  "" cuadno termina
  String palabra;

  if (_posPalabra1 == 0){                                //Primer palabra
    _posPalabra2 = Texto.indexOf(' ');
    palabra  = Texto.substring(0, _posPalabra2);
    _posPalabra1 = _posPalabra2; 
    }  
    else{                                                 //Siguientes
      if (_posPalabra2 < 0) {palabra = "";}                 // si la ultima plabra fue la ultima
      else{
        _posPalabra1 = _posPalabra2 + 1;                  
        _posPalabra2 = Texto.indexOf(' ', _posPalabra1);
        palabra  = Texto.substring(_posPalabra1, _posPalabra2); 
      }
    }
    return palabra;
  }

bool Secuencia::ArmarSecuenciaTexto (String texto, bool info) {
  
  int largo = texto.length();
  int cant = largo / 2;                             //cantidad de pares de letras
  int a = 0;
  bool aux;
  String parXY;
  
  if ( (largo % 2) == 0) SeqCerrada = 1;            // Cambia a _cerrada si es par
  else SeqCerrada = 0;
  
  
  for (int i ; i < cant; i++) {
    parXY = texto.substring(i * 2, + (i * 2) + 2);
    _procesarLetrasXY (parXY, i);
  }
  if (info) {
     Serial.println();
     Serial.print("GENERANDO SECUENCIA: ");
     Serial.println(texto);
     Serial.println("\t [INCX  INCY MODO]");

     for (int cont = 0; cont < cant; cont++) {
      Serial.print("\t [");
      Serial.print( M1[cont]); Serial.print("\t");
      Serial.print( M2[cont]); Serial.print("\t");
      Serial.print( Modo[cont]);
      Serial.println("]");
     }
    if (SeqCerrada == 1) Serial.println ("\t SEQ CERRADA");
    else Serial.println ("\t SEQ ABIERTA");
  }
  largoSeq = cant;


  do {                                                              //reparo la secuencia rotandola si es necesario
    aux =  _repararSecuencia (a);
    a++;
  } while ( aux == false and a <= 3);
  if (aux == false) Serial.println("\t Secuencia invalida");
  largoSeq = cant;
  return aux;
 
}

void Secuencia::seqInit(int dir)
//dejo la secuencia lista para ser recorrida arriba/abajo 
{
  _dir = dir;
  if (_dir == 1) _taskIndex = 0;
  if (_dir == -1) _taskIndex = matrixLenght+1;
  activa = true;
}

void Secuencia::getTask()
// avanza una tarea y deja los valores de la tarea en incX, incY, _modo
{
  if (activa)
  {
    incX = matrixM1[_taskIndex];
    incY = matrixM2[_taskIndex];
    _taskIndex = _taskIndex + _dir; 
        
    Serial.print(" "); 
    Serial.print(_taskIndex - 1);
    

    if ((_taskIndex == 0) or (_taskIndex == matrixLenght +1 )) activa = false;
  }
}

void Secuencia::addTask (long X, long Y, int M)
{
  matrixM1[_taskIndex] =  X;
  matrixM2[_taskIndex] =  Y;
  matrixModo[_taskIndex] = M;
  if (_taskIndex < MAX_SEQ - 1) _taskIndex++;
  matrixLenght = _taskIndex;
 
}
