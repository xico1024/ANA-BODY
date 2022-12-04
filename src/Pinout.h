#ifndef Pinout_h
#define Pinout_h
// 1 define que se usa un driver 
#define motorInterfaceType 1 

// pines motores ruedas
#define DIR3 6
#define STEP3 3
#define DIR4 13
#define STEP4 12

// pines para el brazo y antebrazo
#define DIR1 5
#define STEP1 2
#define DIR2 7
#define STEP2 4
#define ENABLE_MOTR 8     //Ruedas  
#define ENABLE_MOT 9      //Brazo 


#define HSW1  31        //Switch limite Brazo
#define HSW2  29        //Switch limite Antebrazo
#define HSW3  33        //Switch limite Codo
#endif