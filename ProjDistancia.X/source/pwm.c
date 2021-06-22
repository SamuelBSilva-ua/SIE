/* 
 * File:   pwm.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
 */

#include <xc.h>
#include <stdlib.h>
#include <stdint.h>

#include "../inc/pwm.h"


/********************************************************************
* Function: 	PWM2Init(uint8_t timer_number))
* Precondition: 
* Input: 		timer_number to choose timer 1 or timer2
* 
* Returns:      PWM1_SUCCESS if success.
 *              PWM1_XXXX if error (check pwm.h)
* 
* Overview:     Initializes the PWM module.
* 
********************************************************************/	
int PWM2Init(uint8_t timer_number)
{
    /* OC1 pin config - Datasheet PIC32MX, page 24 -  RD0 */
    /* Section 16, Page 62, PWM datasheet tells us pins should be set as output */
    
    // Set RD0 as output, pulled High
    TRISDbits.TRISD1 = 0;
    PORTDbits.RD1 = 1;
    
    /* Control registers */
    /* Section 16, Page 7, PWM datasheet*/
    OC2CONbits.OCM=6; //set OC1 as PWM enable
    OC2CONbits.ON=0; //disable PWM to start configuration
    OC2CONbits.OC32=0; //16 bits time source
    
    switch (timer_number){
        case 2:
            OC2CONbits.OCTSEL=0; //Timer 2
            break;
        case 3:
            OC2CONbits.OCTSEL=1; //Timer 3
            break;
        default:
            return PWM1_INVALID_TIMER; //Invalid timer for OC1
            break;
    }

    OC2R=0; //sets pwm value
    
    OC2CONbits.ON=1; //enable PWM 
    
    return PWM1_SUCCESS;
}

/********************************************************************
* Function: 	PWM2_updateDutyCycle(uint8_t dutycycle,uint8_t timer_number)
* Precondition: 
* Input: 		dutycycle intended.
 *              timer_number is the timer used, 2 or 3.
* 
* Returns:      PWM1_SUCCESS if success.
*               PWM1_XXXX if error (check pwm.h)
* 
* Overview:     Updates PWM duty-cycle.
* 
********************************************************************/	
int PWM2_updateDutyCycle(uint8_t dutycycle,uint8_t timer_number)
{
    //Check if duty cycle is between [0,100]
    if ((dutycycle < PWM1_MINIMUM_DUTYCYCLE) && (dutycycle > PWM1_MAXIMUM_DUTYCYCLE))
        return PWM1_INVALID_DUTYCYCLE;
   
    
    //To change the duty-cycle we must change the OC1RS to the desired value.
    //As duty cycle is the time the signal is on, we need to get a fraction of the
    //timer count to turn on according to the duty cycle.
    //Dutycycle=100%*(OC1R/(PR1+1))
    //Solving in order to OC1R:
    //OC1R=((PR1+1)*dutycycle)/100
    
    
    //Example: 
    //For 50% duty-cycle with PR2=400, OC1RS would be 200
    //For 25% duty cycle with PR2=400, OC1RS would be 100 
    
    
    //Using OC1RS as our goal is to change OC1R on the next pulse so OC1RS gets 
    //loaded to OC1R on the next HIGH.
    
      switch (timer_number){
        case 2:
            OC2RS=((PR2+1)*dutycycle/100); //For timer 2
            break;
        case 3:
            OC2RS=((PR3+1)*dutycycle/100); //For timer 3
            break;
        default:
            return PWM1_INVALID_TIMER; //Invalid timer for OC1
            break;
    }

    return PWM1_SUCCESS;
}



/********************************************************************
* Function: 	PWM2_adjustment(int adjustment, uint8_t timer_number)
* Precondition: 
* Input:        adjustment value.
 *              timer_number is the timer used, 2 or 3.
* 
* Returns:      PWM1_SUCCESS if success.
*               PWM1_XXXX if error (check pwm.h)
* 
* Overview:     Adjustment to the OC1RS for PI controller.
* 
********************************************************************/	
int PWM2_adjustment(int adjustment, uint8_t timer_number)
{
    
      switch (timer_number){
        case 2:
            OC2RS=((PR2+1)+adjustment);
            break;
        case 3:
            OC2RS=((PR3+1)+adjustment);
            break;
        default:
            return PWM1_INVALID_TIMER; //Invalid timer for OC1
            break;
    }

    return PWM1_SUCCESS;
}