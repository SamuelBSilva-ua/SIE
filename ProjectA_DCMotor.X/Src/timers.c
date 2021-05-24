/* 
 * File:   timers.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
 * Overview:
 *          Functions to handle Timers.    
 *
 */


#include <xc.h>
#include <stdlib.h>
#include <stdint.h>

#include "../includes/timers.h"
#include "../includes/general.h"




/********************************************************************
* Function: 	Timer2Init(uint32_t frequency,uint8_t prescaler)
* Precondition: 
* Input: 		frequency of the timer
*               prescaler value
* 
* Returns:      TIMER_SUCESS if success.
*               TIMER_ERROR if failure.
* 
* Overview:     Initializes the Timer2.
* 
********************************************************************/	
int Timer2Init(uint32_t frequency, uint16_t prescaler)
{
    //Assert prescaler is valid
    if ((prescaler > MAXIMUM_PRESCALER) || (prescaler < MINIMUM_PRESCALER)){
        return TIMER_PRESCALER_ERROR;
    }
    
    
    /* TYPE B timer configuration */
    
    /* Control registers */
    /* Section 14, Page 9, Timer datasheet*/
    
    T2CONbits.ON=0; //Turn the timer off before configuration
    T2CONbits.SIDL = 0; //Continue operation in Idle
    
    switch(prescaler){ 
        case 1:
            T2CONbits.TCKPS=0; //Set prescaler to 1:1
            break;
        case 2:
            T2CONbits.TCKPS=1; //Set prescaler to 1:2           
            break; 
        case 4:
            T2CONbits.TCKPS=2; //Set prescaler to 1:4
            break; 
        case 8:
            T2CONbits.TCKPS=3; //Set prescaler to 1:8 
            break; 
        case 16:
            T2CONbits.TCKPS=4; //Set prescaler to 1:16  
            break; 
        case 32:
            T2CONbits.TCKPS=5; //Set prescaler to 1:32 
            break; 
        case 64:
            T2CONbits.TCKPS=6; //Set prescaler to 1:64
            break; 
        case 256:
            T2CONbits.TCKPS=7; //Set prescaler to 1:256 
            break;
        default:
            return TIMER_PRESCALER_ERROR; //Prescaler not valid
            break;
    }
    
    T2CONbits.T32=0; //16 bit counter
    T2CONbits.TCS=0; //Internal peripheral clock
    TMR2=0; //Clear timer register
    PR2=(PBCLK/(frequency*prescaler))-1; //Timer period register
    
    /* Interruptions */
    IEC0bits.T2IE = 0; //no interrupts (polling)
    IFS0bits.T2IF = 0; //reset interrupt flag
    
    
    T2CONbits.ON=1; //Timer configuration complete, turn on timer
    
    return TIMER_SUCESS;
}




/********************************************************************
* Function: 	Timer3Init(uint32_t frequency,uint8_t prescaler)
* Precondition: 
* Input: 		frequency of the timer
*               prescaler value
* 
* Returns:      TIMER_SUCESS if success.
*               TIMER_ERROR if failure.
* 
* Overview:     Initializes the Timer3.
* 
********************************************************************/	
int Timer3Init(uint32_t frequency, uint16_t prescaler)
{
    //Assert prescaler is valid
    if ((prescaler > MAXIMUM_PRESCALER) || (prescaler < MINIMUM_PRESCALER)){
        return TIMER_PRESCALER_ERROR;
    }
    
    
    /* TYPE B timer configuration */
    
    /* Control registers */
    /* Section 14, Page 9, Timer datasheet*/
    
    T3CONbits.ON=0; //Turn the timer off before configuration
    T3CONbits.SIDL = 0; //Continue operation in Idle
    
    switch(prescaler){ 
        case 1:
            T3CONbits.TCKPS=0; //Set prescaler to 1:1
            break;
        case 2:
            T3CONbits.TCKPS=1; //Set prescaler to 1:2           
            break; 
        case 4:
            T3CONbits.TCKPS=2; //Set prescaler to 1:4
            break; 
        case 8:
            T3CONbits.TCKPS=3; //Set prescaler to 1:8 
            break; 
        case 16:
            T3CONbits.TCKPS=4; //Set prescaler to 1:16  
            break; 
        case 32:
            T3CONbits.TCKPS=5; //Set prescaler to 1:32 
            break; 
        case 64:
            T3CONbits.TCKPS=6; //Set prescaler to 1:64
            break; 
        case 256:
            T3CONbits.TCKPS=7; //Set prescaler to 1:256 
            break;
        default:
            return TIMER_PRESCALER_ERROR; //Prescaler not valid
            break;
    }
   
    T3CONbits.TCS=0; //Internal peripheral clock
    TMR3=0; //Clear timer register
    PR3=(PBCLK/(frequency*prescaler))-1; //Timer period register
    
    /* Interruptions */
    IEC0bits.T3IE = 0; //no interrupts (polling)
    IFS0bits.T3IF = 0; //reset interrupt flag
    
    
    T3CONbits.ON=1; //Timer configuration complete, turn on timer
    
    return TIMER_SUCESS;
}




