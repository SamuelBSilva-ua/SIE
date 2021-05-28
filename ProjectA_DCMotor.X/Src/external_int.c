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




void InitExternalInterrupt1(void){
    
    // Configure Int 1 (vector numbers defined in p32mx795512h.h)
    TRISEbits.TRISE8 = 1; // Set pin as input
    INTCONbits.INT1EP = 1; // Generate interrupts on {rising edge-1 falling edge - 0}
    IFS0bits.INT1IF = 0; // Reset int flag
    IPC1bits.INT1IP = 5; // Set interrupt priority (1..7) *** Set equal to ilpx above


    IEC0bits.INT1IE = 1; // Enable Int1 interrupts
}

