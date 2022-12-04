
#include "Limites.h"

Limites::Limites(void) 
//Constructor
{
  pinMode (HSW1, INPUT_PULLUP); // home M1
  pinMode (HSW2, INPUT_PULLUP); // home M2
  pinMode (HSW3, INPUT_PULLUP); // home M2
  }

byte Limites::sw1()
//0=sensor abierto, 1=choco , 2 sigue chocado
{
  bool sw1 = digitalRead(HSW1);
  byte out;
  if (sw1 == 1)   //Libreado
  {
   if (_sw1Ant > 0) delay (10);
    out = 0;
    _sw1Ant = 0;
  }
  else //sw1 == 0  Presionado
  {
   if (_sw1Ant == 0) 
   {
     delay(10);
     out = 1;
     _sw1Ant = 1;
   }
   else if (_sw1Ant == 1)
   {
     out = 2;
   } 
  } 
  return out;

}

byte Limites::sw2()
//0=sensor abierto, 1=choco , 2 sigue chocado
{
  bool sw2 = digitalRead(HSW2);
  byte out;
  if (sw2 == 1)   //Libreado
  {
    if (_sw2Ant > 0) delay (10);
    out = 0;
    _sw2Ant = 0;
  }
  else //sw2 == 0  Presionado
  {
   if (_sw2Ant == 0) 
   {
     delay(10);
     out = 1;
     _sw2Ant = 1;
   }
   else if (_sw2Ant == 1)
   {
     out = 2;
   } 
  } 
  return out;
}

byte Limites::sw3()
//0=sensor abierto, 1=choco , 2 sigue chocado
{
  bool sw3 = digitalRead(HSW3);
  byte out;
  if (sw3 == 1)   //Libreado
  {
    if (_sw3Ant > 0) delay (10);
    out = 0;
    _sw3Ant = 0; 
  }
  else //sw3 == 0  Presionado
  {
   if (_sw3Ant == 0) 
   {
     delay(10);
     out = 1;
     _sw3Ant = 1;
   }
   else if (_sw3Ant == 1)
   {
     out = 2;
   } 
  } 
  return out;

}

/*
bool Limites::anySW(){
  bool a = true;  
  if (sw1() > 0){a = false;}
  if (sw2() > 0){a = false;}
  if (sw3() > 0){a = false;}
  return a;
}
*/

long Limites::M1Inf(long currentM2)
//Limites Absolutos
{ 
  long M1 =  currentM2/1.2;
  return M1; 
  }

long Limites::M1Sup(long currentM2)
//Limites Absolutos
{
  long M1;
  if (currentM2 <= 690)  M1 = ((currentM2/1.2) + 670); 
  if (currentM2 >  690)  M1= 1200;
  return M1;
  }

long Limites::M2Inf(long currentM1)
//Limites Absolutos
{
  long M2;
  if (currentM1 <= 670) M2 = 0; 
  if (currentM1 >  670 and currentM1 <  1400) M2 = 1.2*(currentM1 - 670); 
  return M2;
  }
  
long Limites::M2Sup(long currentM1)
//Limites Absolutos
{
  long M2 = 1.2 * currentM1;
  return M2;
  }

int Limites::antebrazoSW(){                                           
//verifica limite del antebrazo, 0 = OK 1 = Choco
  int estadoOut = 0;
  if (digitalRead(HSW3) == 0) {                                                     // HSW3 = 0 = choco                                
    if  (_HSW3Ant == 1) { estadoOut = 1; _HSW3Ant = 0;}  //AGREGAR DELAY*********                                         
  }
  else {                                                                          
    if (_HSW3Ant == 0) delay(50);                                                   //solatndo el swithc delay 50 para debounde                                                                                                                                                             
    estadoOut = 0; _HSW3Ant = 1;
  }
  return estadoOut;
}

bool Limites::armConstrain (long actualM1, long actualM2, long incM1, long incM2, int info) {    //devuelve false si falla                                                   //  TRUE = OK, le paso los incrementos
  //Validacion  de posiciones del brazo: M1 Hombro, M2 Codo
  bool result = true;
  long newM1 = actualM1 + incM1;                                                       //nueva posicion X
  long newM2 = actualM2 + incM2;                                                       //nueva posicion Y

  if ((newM1 < M1Inf(actualM2)) or (newM1 > M1Sup(actualM2)))  result = false ;
  if ((newM2 < M2Inf(actualM1)) or (newM2 > M2Sup(actualM1)))  result = false ;
  if ((info > 1) and  (_INFO == 1)) {
  if (!result) {
     
      Serial.println ("\n\t\t ARM CONSTRAIN ERROR:");
      Serial.print ("\t\t Actual M1, M2: \t"); Serial.print (actualM1); Serial.print (", "); Serial.println ( actualM2);
      Serial.print ("\t\t Inc M1, M2:  \t\t"); Serial.print (incM1); Serial.print (", "); Serial.println(incM2);
      Serial.print("\t\t Limites M1("); Serial.print(actualM2); Serial.print("): \t"); Serial.print(M1Inf(actualM2)); Serial.print(" - "); Serial.print(M1Sup(actualM2));Serial.print(" ["); Serial.print (newM1);Serial.print("]");
      if (newM1 > M1Sup(actualM2)  or newM1 < M1Inf(actualM2)) Serial.print(" ** \n"); else Serial.print("\n");
      
      Serial.print("\t\t Limites M2("); Serial.print(actualM1); Serial.print("): \t");Serial.print(M2Inf(actualM1)); Serial.print(" - "); Serial.print(M2Sup(actualM1));Serial.print(" ["); Serial.print (newM2);Serial.print("]");
      if (newM2 > M2Sup(actualM1)  or newM2 < M2Inf(actualM1)) Serial.print(" ** \n"); else Serial.print("\n");
      Serial.println();
    }

  }
  //if (!result) Sonido.Hablar("HAAA!");
  return result;
}

bool Limites::inside(long motor, long currentM1, long currentM2)
//Devuelve false si esta fuera de los limites o si toco algun sensor
{                                                                                  // TRUE si esta en cond de moverse,  FALSE llego al limite
  bool result = true;
  if (motor == 1){
    if ((currentM1 <= M1Inf(currentM2)) or (currentM1 >= M1Sup(currentM2)))  result = false ;
   // result = AnySW() ;        // si algun switch se cerro devuelve false
  }
  if (motor == 2){
    if ((currentM2 <= M2Inf(currentM1)) or (currentM2 >= M2Sup(currentM1)))  result = false ;
   //result = AnySW() ;
  }
  return result;
}
