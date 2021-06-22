/* 
 * File:   ADC.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
 * Overview:
 *          Functions to handle ADC.    
 *
 */

#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../inc/ADC.h"
#include "../inc/general.h"


volatile float result_ADC;

/********************************************************************
* Function: 	ADC1Init()
* Precondition: 
* Input: 		-
* Returns:      ADC_SUCCESS if Ok.
*               ADC_XXX error codes in case of failure (see ADC.h)
* Overview:     Initializes ADC module with the inteended channel.
*	
* 
********************************************************************/	

uint8_t ADC1Init(void){
      
    /* Control registers */
    /* Section 17, Page 7, ADC datasheet*/
    AD1CON1bits.ON = 0; //Disable ADC
    
    DDPCONbits.JTAGEN = 0;  // Disable JTAG interface as it uses a few ADC ports
    
    /* Implementation method */
    /* Section 17, Page 26, ADC datasheet*/
   
    TRISBbits.TRISB1 = 1; // Set RB1 to input mode
    /* Implementation method A-1*/
    AD1PCFGbits.PCFG1 = 0; // Set AN1 to analog mode
    /* Implementation method B-1*/
    AD1CHSbits.CH0SA = 1; // Select AN1 as input for A/D converter
    
    /* Implementation method */
    /* Section 17, Page 26, ADC datasheet*/
    /* Implementation method C-1*/
    AD1CON1bits.FORM = 0; // Integer 16 bit output format
    /* Implementation method C-2*/
    AD1CON1bits.SSRC = 2; // Timer 3 period match ends sampling and starts conversion
    /* Implementation method D-1*/
    AD1CON2bits.VCFG = 0; // VR+=AVdd; VR-=AVss
    /* Implementation method D-2*/
    AD1CON2bits.CSCNA=0; //Do not scan inputs
    /* Implementation method D-3*/
    AD1CON2bits.SMPI = 0; // Number (+1) of consecutive conversions, stored in ADC1BUF0...ADCBUF{SMPI}
    /* Implementation method D-4*/    
    AD1CON2bits.BUFM=0; // Use 16 word buffer
    /* Implementation method D-5*/  
    AD1CON2bits.ALTS=0; //Only the inputs by CH0SA are selected for sampling (selecting the MUX)
    /* Implementation method E-1*/  
    AD1CON3bits.ADRC = 1; // ADC uses PBCLOCK 
    /* Implementation method E-2*/  
    AD1CON3bits.SAMC = 16; // Sample time is 16TAD ( TAD = 100ns)
    /* Implementation method E-3*/  
    AD1CON3bits.ADCS = 0; // ADC uses internal RC clock, no pre-scaler
    
        
    /* Aditional control registers */        
    AD1CON1bits.SIDL=0; // Continue operation on IDLE mode
    AD1CON1bits.CLRASAM = 1; //Stop conversion when 1st A/D converter interrupt is generated and clears ASAM bit automatically
    
    
    // Enable module
    AD1CON1bits.ON = 1; // Enable ADC
    
    return ADC_SUCCESS;
}


/********************************************************************
* Function: 	ADC1Read()
* Precondition: -
* Input: 		-
* Returns:      ADC1BUF0 value is returned.
* Overview:     Reads a sample from the ADC.
*	
* 
********************************************************************/
float ADC1Read(void){

    
    IFS1bits.AD1IF = 0;
	AD1CON1bits.ASAM = 1;
    while(IFS1bits.AD1IF == 0);
    
    result_ADC = (ADC1BUF0 * 3.3) / 1023;
    
    return result_ADC;
}

