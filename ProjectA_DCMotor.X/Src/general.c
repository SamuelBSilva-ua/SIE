/* 
 * File:   general.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
*/

#include "../includes/general.h"
#include <xc.h>


/********************************************************************
* Function: 	PinConfig()
* 
* Input: 		-
* 
* Returns:      -
* 
* Overview:     Initializes all the Pins needed.
* 
********************************************************************/
void PinConfig(void){
    
    TRISAbits.TRISA3 = 0; //LED on pic
    
    //Pin Config
    TRISEbits.TRISE5 = 0;  //RE5 set as output - 1A l293
    TRISEbits.TRISE6 = 0;  //RE6 set as output - 2A l293

    TRISEbits.TRISE7 = 1;  //RE7 set as input  - Channel B Motor encoder
    TRISEbits.TRISE8 = 1;  //RE8 set as input  - Channel A Motor encoder    

}


/********************************************************************
* Function: 	InterfaceMenu()
* 
* Input: 		-
* 
* Returns:      -
* 
* Overview:     Prints the User interface menu.
* 
********************************************************************/
void InterfaceMenu(void){
    PutString("------------------ Projeto Motor DC ----------------------- \n\n\r");
    PutString("Definir o sentido de rotacao para a direita: introduzir r/R \n\r");
    PutString("Definir o sentido de rotacao para a esquerda: introduzir l/L  \n\r");
    PutString("Definir velocidade: introduzir um numero entre 10 e 50 rpm \n\r");
    PutString("Fast-stop: introduzir f/F  \n\r");
    PutString("Paragem do motor: introduzir '0'  \n\r");
    PutString("Tuning do parametro Kp do controlador PI: introduzir p/P \n\r"); 
    PutString("Tuning do parametro Ki do controlador PI: introduzir i/I \n\r");
    PutString("\n\r");
}