#include "Dibujar.h"

Dibujar::Dibujar(void) : Manox3() {} // Constructor hereda Manox3
      
void Dibujar::seqForma(long nTrazos, int mode)
{ // Arma una secuencia

  long posOrigen1 = brazo.currentPos(1);
  long posOrigen2 = brazo.currentPos(2);

  Serial.println();
  Serial.println("NUEVA FORMA (SEQ)");
  Serial.println("-----------------");
  Serial.print("\t Trazos:\t");
  Serial.println(nTrazos);
  Serial.print("\t PosActual:\t");
  Serial.print(posOrigen1);
  Serial.print(", ");
  Serial.println(posOrigen2);

  secuencia.setOrigen(posOrigen1, posOrigen2);
  secuencia.ArmarSecuencia(nTrazos, mode, true);
  nuevaSeq = true;
}

void Dibujar::pintar(long incX, long incY, long iteraciones, bool info)
{

  /*
      //Velocidad y aceleracion / sacar de aqui
      long A1 = random(AVMIN, AVMAX); long V1 = random(AVMIN, AVMAX);
      long A2 = random(AVMIN, AVMAX); long V2 = random(AVMIN, AVMAX);
      brazo.AVset(1, A1 / 2, V1 / 2);
      brazo.AVset(2, A2 / 2, V2 / 2);
      int cont = 0;

      if (info == true) {
        Serial.println();
        Serial.println("\t LISSAJOUS");
        Serial.print ("\t PathX: "); Serial.print (brazo.currentPos(1)); Serial.print (" -- "); Serial.println (brazo.currentPos(1) + incX);
        Serial.print ("\t PathY: "); Serial.print (brazo.currentPos(2)); Serial.print (" -- "); Serial.println (brazo.currentPos(2) + incY);
        Serial.print ("\t Iteraciones: "); Serial.println (iteraciones);
        Serial.print ("\t Vel: "); Serial.println((V1 + V2) / 4);
      }
      //verifico que pueda moverme con ambos motores
      if (limite.armConstrain(brazo.currentPos(1),brazo.currentPos(2),incX, incY, 2)){
      stepper1.move(incX);
      stepper2.move(incY);

      do {
        stepper2.run();
        if (((stepper2.distanceToGo() == 0) or limite.AntebrazoSW() > 0)) {                                                 //alcanzo destino o choco con el SW
          Serial.println("INVIERTE Y");
          incY = -incY; //invierto el destino
          stepper2.move(incY);
          cont ++;
        }

        stepper1.run();
        if (((stepper1.distanceToGo() == 0) or limite.limiteAntebrazoSW()) > 0) {
         Serial.println("INVIERTE X");
          incX = -incX; //invierto el destino
          stepper1.move (incX);
          cont ++;
        }

      } while (cont < iteraciones);
      }

      if (info == true) {
        Serial.print("\t ITERACION  ");
        Serial.println (cont);
      }
      Serial.println();
   */
}

void Dibujar::dibujaTexto(String Texto)
{ // Arma una secuencia de mobimientos segun el Texto
  String palabra;
  long posOrigen1;
  long posOrigen2;
  do
  {
    posOrigen1 = brazo.currentPos(1);
    posOrigen2 = brazo.currentPos(2);
    palabra = secuencia.proximaPalabra(Texto); // voy avanzando de a una palabra

    secuencia.setOrigen(posOrigen1, posOrigen2); // Solo para probar
    dibujarPalabra(palabra);
    seqForma(1, 3);    // Creo la secuecnia  (nTrazos,  modo)
    runSeq(100, 1, 0); // muevo el brazo  (tamano, intensidad,  lapizup)
    delay(500);
  } while (palabra != "");
}

void Dibujar::dibujarPalabra(String Palabra)
// Arma una secuencia de mobimientos segun la plabra, y si es valida la ejecuta
{
  if (secuencia.ArmarSecuenciaTexto(Palabra, true)) runSeq(100, 4, 1); 
  else Serial.println("\t No se ejecuto la secuencia");
}

void Dibujar::runSeq(long tamano, int intensidad, int dibuja)
{
  int cont;
  int contInt;
  long mAux;
  long posOrigen1 = brazo.currentPos(1);
  long posOrigen2 = brazo.currentPos(2);

  Serial.println();
  Serial.print("EJECUTANDO SECUENCIA intensidad[");
  Serial.print(intensidad);
  Serial.println("] ");

  if (dibuja == 0) manoColor.lapizUp();
  if (dibuja == 1) manoColor.lapizDown();
  contInt = 0;
  do // repeticion x intensidad
  {
  cont = 0;
  secuencia.seqInit(1);
  do // repeticion secuencia de c trazos
    {
      secuencia.getTask();
      if (secuencia.modo < 10)
      {
          brazo.mover(secuencia.incX, secuencia.incY, tamano, 5, false);
          //brazo.mover(secuencia.matrixM1[cont], secuencia.matrixM2[cont], tamano, secuencia.matrixModo[cont], false);
      }
      cont = cont + 1;
      Serial.print("+");
      if (secuencia.Modo[cont] >= 6)
        contInt = intensidad; // PARA SALIR LUEGO DE UNA SOLA **** VERRRRR

    } while (cont < secuencia.matrixLenght);

   
  contInt = contInt + 1; // Primer pasada
  // Retorno a  origen  Aca el factor de escala hace que la figura cierre o se desplace
  if (secuencia.SeqCerrada == 0)
  { // vuelvo sobre mi trazo
    if (contInt < intensidad)
    {
      cont = cont - 1; // ultimo paso
      //vuelvo en modo 3 o 4  dependiendo cual es mejor
      secuencia.seqInit(-1); //direccion negativa
      do
      {
        secuencia.getTask();
        //mAux = secuencia.Modo[cont];
        mAux = secuencia.modo;
        if (mAux == 3) mAux = 4;
        else if (mAux == 4) mAux = 3;

        brazo.mover(-secuencia.incX, -secuencia.incY, tamano, 5, false);  
       // brazo.mover(-secuencia.M1[cont], -secuencia.M2[cont], tamano, mAux, false);
        cont = cont - 1;
        Serial.print("-");
      } while (cont >= 0);
      }
      
    }
  else
    {
      // vuelvo en modo 5 Linea recta
      if (contInt < intensidad)
        brazo.mover(posOrigen1 - brazo.currentPos(1), posOrigen2 - brazo.currentPos(2), 100, 5, false);
    }
  Serial.print("  [");  
  Serial.print(contInt);
  Serial.println("]");
 
} while (contInt < intensidad);

  Serial.println();
  Serial.print("CURRENT POS: ");
  Serial.print(brazo.currentPos(1));
  Serial.print(" ");
  Serial.println(brazo.currentPos(2));
}

void Dibujar::posActual()
{
  posM1 = brazo.currentPos(1);
  posM2 = brazo.currentPos(2);
}

void Dibujar::parking()
{
  brazo.goHome();
}

String Dibujar::exportSeq()
{
  String buffer;
  int cont;
  do
  {
    buffer = buffer + String(secuencia.M1[cont]) + ".";
    buffer = buffer + String(secuencia.M2[cont]) + ".";
    buffer = buffer + String(secuencia.Modo[cont]) + "|";
    cont++;
  } while (cont < secuencia.largoSeq);

  return buffer;
}

void Dibujar::zonaCarga(){
  brazo.cargarPintura(2);
}

void Dibujar::loop(){
  brazo.run();
}
