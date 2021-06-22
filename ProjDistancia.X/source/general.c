/* 
 * File:   general.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
*/

#include "../inc/general.h"
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
    //Set RE8 as output (to drive the LED)
    TRISEbits.TRISE8 = 0;
    PORTEbits.RE8 = 0; //turn off LED
}
