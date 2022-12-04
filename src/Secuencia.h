#ifndef Secuencia_h
#define Secuencia_h

#define MAX_SEQ 11 
#define _maxMode 4  //antes 8
#define _minMode 1

#include "Limites.h"


class Secuencia
{
  public:
    Secuencia(void);
    long posInicial1, posInicial2;
    long posActual1, posActual2; 
    long incX,incY;
    int SeqCerrada;                                                       //1 si, 0 NO
    int largoSeq;                                                         //largo de la sequencia
    
    bool activa;                                                                                            
    int modo;
    long M1 [MAX_SEQ];                                                     //_incX
    long M2 [MAX_SEQ];                                                     //_incY
    int Modo [MAX_SEQ];                                                    //Modo Dibujo
    
    long matrixM1 [MAX_SEQ];                                                     //_incX
    long matrixM2 [MAX_SEQ];                                                     //_incY
    int matrixModo [MAX_SEQ]; 
    int matrixLenght;

    int valoracionInt[MAX_SEQ];                                           //Basada en simetria y numero aureo
    int animo[MAX_SEQ];                                                   //estado endorfinico actual, dopamina
    int valoracionExt[MAX_SEQ];                                           //valoracion del tutor
    int animoExt[MAX_SEQ];                                                //estado de animo del turor
 
    void seqInit(int dir);
    void getTask();                                                      //deposita en _incX _incY _modo
    void addTask (long X, long Y, int M);                                        //agrega una tarea a la secuencia
    void setOrigen (long pIni1, long pIni2);
    void ArmarSecuencia (long nTrazos, int mode, bool info);               // modo 0 = RANDOM
    bool ArmarSecuenciaTexto (String texto, bool info);
    String proximaPalabra(String Texto);
    
   
  private:
    byte _info = 1;                                               // 1 loguea en pantalla, 0 desactivado
    int _cerrada;
    int _modo, _taskIndex, _dir;
    long _incX, _incY;                                            //incremento Motor1, Motor2                                                                                                                      
    long _incTotalX, _incTotalY;
    long _randomGen (int motor,long xInicial, long yInicial);
    long _maxMotor(int motor, long inc, long actualM1, long actualM2); //busca el maximo en la direccion "inc" de "motor"
    void _procesarLetrasXY (String Texto,int orden);
    bool _validarSecuencia (bool info);
    bool _repararSecuencia (int modo);
  
    int _posPalabra1;
    int _posPalabra2;
    long _posOrigen1;                                      // __incXinicial de la secuencia
    long _posOrigen2;  
    long _limiteSup;                                       //para calculo de constrain
    long _limiteInf;                                       //para calculo de constrain

    Limites limite; 
};
#endif
