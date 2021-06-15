/* 
 * File:   external_int.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
 * Overview:
 *          Functions to handle external interrupt.    
 *
 */

#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/attribs.h>

#include "../includes/external_int.h"



/********************************************************************
* Function: 	InitExternalInterrupt1()
* Precondition: -
* Input: 		-
* 
* Returns:      
 *              
* 
* Overview:     Inits External Interrupt 1.
* 
********************************************************************/	
void InitExternalInterrupt1(void){
    
    IEC0bits.INT1IE = 0; // Enable Int1 interrupts
    
    // Configure Int 1 (vector numbers defined in p32mx795512h.h)
    TRISEbits.TRISE8 = 1; // Set pin as input
    INTCONbits.INT1EP = 1; // Generate interrupts on {rising edge-1 falling edge - 0}
    IPC1bits.INT1IP = 5; // Set interrupt priority (1..7) *** Set equal to ilpx above
    IPC1bits.INT1IS = 1; 
    IFS0bits.INT1IF = 0; // Reset int flag
    
    
    IEC0bits.INT1IE = 1; // Enable Int1 interrupts

    
}

