#include "Arduino.h"
#include "Dibujar.h"
#include "Sonido.h"
#include "Cabeza.h"
#include "Geoloc.h"
#include "Ruedas.h"
#include <TimerOne.h>
#include <Wire.h>

bool I2CdataReady;
volatile unsigned long ISRCount = 1;   // use volatile for shared variables
String posM12Temp, posM12;             // string con la info de pos M1 y M2
String distSensorTemp, distSensor;
int id = 0;                            // para la mensajeria con ESP

String dataI2C;                        // Para los mensajes MEGA a ESP
String I2Cdata;                        // enviado por ESP recibido por MEGA

//Creo los objetos
Ruedas ruedas;
Dibujar Dibujo;
Sonido sonido;
Cabeza cabeza;
Geoloc sensorBrazo;

void clearScreen()
{
  for (int a = 0; a < 40; a++)
  {
    Serial.println();
  }
}

void menu()
{

  Serial.println("MENU");
  Serial.println("----");
  Serial.println();
  Serial.println("u - Menu");
  Serial.println("h - Home");
  Serial.println("r - Mover ruedas modos 1 2 3 4, dist");
  Serial.println("b - Mover ruedas bailar dis1, vel1, dist2, vel2");
  Serial.println("m - Mover X Y");
  Serial.println("p - Lisajous X Y  ****/NO");
  Serial.println("s - Secuencia cantTrazos");
  Serial.println("t - Secuencia de Texto");
  Serial.println("n - Navegar Recorre los bordes");
  Serial.println("l - LapizDown 1 - 0");
  Serial.println("c - Mano Color  1,2,3");
  Serial.println("i - INFO Posicion actual, color etc");
  Serial.println("C - Color Cabeza R G B  0 - 1000");
  Serial.println("M - Modo Cabeza m(0,1,2,3) f(0-100) p(0-100)"); // modo 0 Off, ,1 ON, 2 PULSE, 3 PULSE SECUENCIAL  FADE 0 100  Periodo 0 100
  Serial.println();
  
}

void I2CreceiveData(int howMany)                    // ISR MEGA recibe data del ESP (que viene por MQTT)
{
  // Los escribe en I2Cdata, I2CdataReady = true
  I2Cdata = "";
  while (0 < Wire.available())
  {
    char c = Wire.read();
    I2Cdata = I2Cdata + c; // armo el comadno byte a byte
  }
  I2CdataReady = true; // para ser procesado
}

void MEGAsendDataI2C()                              // MEGA responde  a la solicitud de ESP via I2CreceiveData
{
  // Se envian una serie de parametros de estado delimitados por n@ PAYLOAD @
  // Switch ID
  switch (id)
  {
    case 0:
    dataI2C = String(id) + "@" + posM12 + "@1";      //1@ informacion de posiciones M1 y M2   @1 hay mas
    break;

    case 1:
    dataI2C = String(id) + "@" + distSensor + "@1";  //2@ informacion de distancia a la pared
    break;

    case 2:
    cabeza.envioColorI2C(10,20,30);
    dataI2C = String(id) + "@" + cabeza.color + "@1"; //3@ informacion de color
    cabeza.cambio_color = false;
    break;

    case 3:
    cabeza.envioModoI2C(1,2,3);
    dataI2C = String(id) + "@" + cabeza.modo + "@";   //4@ informacion de modo    @0 no hay mas
    cabeza.cambio_modo = false;
    break;
    
    /*
    case 4:
    dataI2C = String(id) + "@" + "/MATRIX/#";
    dataI2C = dataI2C + "SEQUENCIA" + "#" + "@0"; // la secuencia termina en #
    //dataI2C = dataI2C + Dibujo.exportSeq() + "#" + "@0"; // la secuencia termina en #
    Dibujo.nuevaSeq = false;
    break;
    */

  /*  //CUSTOM MSGE
      // int valor=35;
      dataI2C = "/FM/POS";
      dataI2C = dataI2C + "#" + posString + "#" ;
      Serial.println(dataI2C);

  */



  }
  char buffer[32];

  dataI2C.toCharArray(buffer, 32);
  Wire.write(buffer);
  id = id + 1;
  if (id > 10) id = 0;  //hasta diez menajes
  /*
  if (Dibujo.nuevaSeq)
  {
    dataI2C = "/MATRIX/";
    dataI2C = dataI2C + Dibujo.exportSeq(); // la secuencia termina en #
    // Serial.println(dataI2C);
    dataI2C.toCharArray(buffer, 32);
    // Wire.write(buffer);
    Dibujo.nuevaSeq = false;
  }
  */

}

void getDataISR(void)                               // Capturo datos de los motores, secuencia, etc..
{
    ISRCount = ISRCount + 1;
    Dibujo.posActual(); // Capturo 3 valores sucesivos de los momtores M1 y M2
    posM12Temp += String(Dibujo.posM1) + "," + String(Dibujo.posM2) + "|";
    //distSensorTemp = distSensorTemp + String(sensorBrazo.Medir()) + "|";
    //Aqui se arman los strings con los datos

    if (ISRCount > 3) 
    {
      posM12 = posM12Temp;
      distSensor = distSensorTemp;
      posM12Temp = "";
      distSensorTemp = "";
      ISRCount = 1;
    }

}

String leerComandoSERI2C()                          // Comandos desde puerto Serial r i2C(IOT) -  NO BLOCK
{
  String comandoSERI2C = "";

  //Serial.print("cmd: ");
 // do { } while ((Serial.available() == false) and (I2CdataReady == false));
  if (Serial.available()) comandoSERI2C = Serial.readString();
  if (I2CdataReady) comandoSERI2C = I2Cdata; // asigno el comando  creado en la interrupcion i2c
  return comandoSERI2C;
}

void procesarComando(String CommandStr)             // Procesa el comando recibido por SERIAL o I2C
{
  char comando, cmd;
  String strAux;
  int val1, val2, val3, val4, val5, val6;
  int Tiempo;
  int c1, c2;
  int ProcesarMovimiento;

  cmd = CommandStr.charAt(0);                           //los comandos son de una letra
  c1 = CommandStr.indexOf(',');
  
  
  if (c1 == -1)//un solo parametro 
  { 
    strAux = CommandStr.substring(1, CommandStr.length());
    val1 = strAux.toInt();
  }

  if (c1 > 0) // mas de un parametro
  {
    strAux = CommandStr.substring(1, c1); val1 = strAux.toInt();

    c2 = CommandStr.indexOf(',', c1 + 1);
    strAux = CommandStr.substring(c1 + 1, c2); val2 = strAux.toInt();

    c1 = c2;
    c2 = CommandStr.indexOf(',', c1 + 1); strAux = CommandStr.substring(c1 + 1, c2);
    val3 = strAux.toInt();

    c1 = c2;
    c2 = CommandStr.indexOf(',', c1 + 1); strAux = CommandStr.substring(c1 + 1, c2);
    val4 = strAux.toInt();

    c1 = c2;
    c2 = CommandStr.indexOf(',', c1 + 1); strAux = CommandStr.substring(c1 + 1, c2);
    val5 = strAux.toInt();

    c1 = c2;
    c2 = CommandStr.indexOf(',', c1 + 1); strAux = CommandStr.substring(c1 + 1, c2);
    val6 = strAux.toInt();
  
  }

  switch (cmd)
  {
  case 's'://secuencia n
  {
    comando = 's'; // simple comilla por que es CHAR !!
    // strAux  = CommandStr.substring(1, CommandStr.length());
    Tiempo = val1;
    ProcesarMovimiento = 1;
    break;
  }

  case 'h':// Estaciono el Brazo
  {
    delay(500);
    Dibujo.parking(); 
    ProcesarMovimiento = 0;
    break;
  }

  case 'i': // Info Status
  {
    Dibujo.posActual();
    clearScreen();
    Serial.println("General Status");
    Serial.println("--------------");
    Serial.print("Pen Status: ");
    Serial.println(Dibujo.pStatus()); //**************************************
    Serial.print("Pen Color: ");
    Serial.println(Dibujo.pColor());
    Serial.print("Posicion: ");
    Serial.print(Dibujo.posM1);
    Serial.print(", ");
    Serial.print(Dibujo.posM2);

    Serial.println();
    Serial.println("Presione Enter para continuar");
    do
    {
    } while (Serial.available() == false);
    menu();
    break;
  }

  case 'u': //Menu
  {
    menu();
    break;
  }

  case 't':// Secuencia de Texto
  { 
    String texto;
    Serial.println("Ingrese texto: ");
    do
    {
    } while (Serial.available() == false);
    if (Serial.available())
      texto = Serial.readString();

    // for (int a=0 ; a<50 ; a++){

    Dibujo.luzOff();
    Dibujo.seqForma(1, 3);    // Creo una secuencia de 1 trazo en modo 3 para desplazarme
    Dibujo.runSeq(100, 1, 0); // muevo el brazo  (tamano, intensidad,  lapizup)
    delay(500);
    Serial.println(texto);
    Dibujo.luzOn(255);
    Dibujo.dibujaTexto(texto); // muevo el brazo  (nTrazos, tamano, intensidad, modo, lapizDown)
    delay(500);
    menu();
    break;
  }
  case 'C':  //color RGB Cabeza
  {
    int r = val1;
    int g = val2;
    int b = val3;

    cabeza.envioColorI2C(r, g, b); // envio color a la cabeza
    cabeza.cambio_color = true;
    break;
  }
  case 'r':  //mover Ruedas
  {
    ruedas.MoverRuedas(val1, val2, val3, val3);
    break;
  }
  case 'b':  //mover Ruedas bailar
  { 
    ruedas.Bailar(val1, val2, val3, val4, val5, val6);
    break;
  }
  case 'M':  //Modo Cabeza
  {
    int m = val1;
    int p = val2;
    int f = val3;

    cabeza.envioModoI2C(m, p, f); // envio modo a la cabeza
    cabeza.cambio_modo = true;
    break;
  }
  case 'l':  //Lapiz UP-Down
  {
    if (val1 == 1)
      Dibujo.lapizDown();
    if (val1 == 0)
      Dibujo.lapizUp();
    ProcesarMovimiento = 0;
    break;
  }
  case 'm':  //Mover Brazo
  {
    comando = 'm';
    // Pos1 = val1;
    // Pos2 = val2;
    //Brazo.Mover(val1, val2, 100, 5, false);
    //Dibujo.CargarPintura(2);
 
    Dibujo.zonaCarga();
 
    
    break;
  }

  case 'c':  //Colr Lapiz
  {
    int c = val1;
    Dibujo.setColor(c);
    break;
  }

  default:
  {
    // statements
    break;
  }
  }

  if (ProcesarMovimiento == 1)
  {
 
    if (comando == 'p')
    { //------------------------------------------------------------------- seqForma
      /*   if (limite.armConstrain(stepper1.currentPosition(),stepper2.currentPosition(),Pos1, Pos2, true) == true) {
           //if (random(1,100) > 50) { Mano.proximoColor(true);delay(2000);} //Cambio color
           Mano.lapizDown(false);
           Brazo.Pintar ( Pos1, Pos2, random(1, 30), true);
           ProcesarMovimiento = 0;
         }*/
    }

    if (comando == 's')
    { //------------------------------------------------------------------- Secuencia
      int t = 0;
      while (t < Tiempo)
      {

        Serial.println("NUEVO DIBUJO");

        if (random(1, 100) > 30)
        { // cambio color de la mano
          Dibujo.proximoColor();
          delay(1000);
        }

        Dibujo.luzOff();
        Dibujo.seqForma(1, 3);  // Creo una secuencia de 1 trazo en modo 3
        Dibujo.runSeq(100, 1, 0); // muevo el brazo  (tamano, intensidad,  lapizup)

        delay(500);
        Dibujo.luzOn(255);
        Dibujo.seqForma(random(1, 6), random(1, 10));   // muevo el brazo  (nTrazos, modo)
        Dibujo.runSeq(random(10, 100), random(1, 10), 1); // muevo el brazo  (tamano, intensidad,  lapizDown
        Serial.println(Dibujo.exportSeq());
        delay(500);
        t = t + 1;
      }

      Dibujo.luzOff();
      ProcesarMovimiento = 0;
    }
  }
  if (I2CdataReady == true)
    I2CdataReady = false;
}

void leerProcesarCmd()                              // Loop para esperar comandos y procesarlos - NO BLOCK
{
  String aux;
  aux = leerComandoSERI2C();
  if (aux != "")
  {
    Serial.println(aux);
    procesarComando(aux);
    Serial.print("cmd: ");
  }
}

//**************
//**************

void setup()
{
  randomSeed(analogRead(0)); // Altero la semilla del Random

  Timer1.initialize(333333);          // inicializo interrupcion 1 seg = 1000000  (1/3 de seg)
  Timer1.attachInterrupt(getDataISR); // rutina para obtener info de estado periodicamente

  Serial.setTimeout(250);
  Serial.begin(115200);

  Wire.begin(8);                         // inicializo I2c Address 8 (MEGA SLAVE)
  Wire.onRequest(MEGAsendDataI2C);       // callback i2c para enviar Info al Controller cuando lo solicite
  Wire.onReceive(I2CreceiveData);     // callback i2C para recepcion de info desde el Head Controller
  // cabeza.intI2C();

  // Sonido.Hablar("HOLA MUNDO, YO SOY ANA");
  delay(100);
  clearScreen();                         // limpio la pantalla;
  Serial.println("*** A.N.A ***");
  // Estaciono el Brazo
  Dibujo.colorInit();
  delay(200);
  Dibujo.lapizUp();
  Dibujo.parking();

  clearScreen();
  menu();
  digitalWrite(ENABLE_MOT, HIGH); // Desactivo motores
}

void loop()
{
  leerProcesarCmd();                                    // hay que llamarlo una vez por vuelta
  ruedas.motoresRun();
  Dibujo.loop();
 // Brazo.DesactivarMotores(20);
}
