/* 
 * File:   PIController.c
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 * 
*/


#include "../includes/PIController.h"
#include "../includes/pwm.h"
#include "../includes/uart.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

/* Controller values */
volatile float Kp_value=0.0025;     //Kp value tested and working
volatile float Ki_value=0.05;       //Ki value tested and working
volatile float previous_error=0;


/********************************************************************
* Function: 	PI_Execute()
* Precondition: 
* Input: 		Setpoint and current RPM
* Returns:      -
*
* Overview:     PI controller for motor RPM to reach the setpoint.
*		
********************************************************************/
void PI_Execute(uint8_t setpoint, uint8_t rpm)
{

	float error = 0;
	float pterm = 0, iterm = 0;
    int adjustment;
    
    /* Error calculation */
    error = setpoint - rpm;
    
    /* Error Sum for integrator */
    previous_error += error;

    /* Proporcional term */
    pterm=Kp_value * error; 
    
    /* Integral term*/
    iterm= previous_error*Ki_value;
    
    
	// calculate the control variable
	adjustment= (int)(pterm + iterm);
     
    
    //saturation  
    if((PR2+adjustment) > PR2){ //if adjustment needed causes value greater than PR2 value, saturation to PR2
        PWM1_updateDutyCycle(100,2); //set duty-cycle to 100%
        adjustment=0;
    }else if((PR2-adjustment) < 0){ //if adjustment needed is causes value less than 0, saturation to 0
        PWM1_updateDutyCycle(0,2); //set duty-cycle to 0%
        adjustment=0;
    }else{
        PWM1_adjustment(adjustment,2); //Adjust OC1RS counter
    }     
}

/********************************************************************
* Function: 	UpdateKp()
*
* Input: 		Kp value
* Returns:      -
*
* Overview:     Sets the Kp value received from user interface.
*		
********************************************************************/
void UpdateKp(float Kp){
    Kp_value=Kp;
}


/********************************************************************
* Function: 	UpdateKi()
*
* Input: 		Ki value
* Returns:      -
*
* Overview:     Sets the Ki value received from user interface.
*		
********************************************************************/
void UpdateKi(float Ki){
    Ki_value=Ki;
}

/********************************************************************
* Function: 	PI_Clear()
*
* Input: 		-
* Returns:      -
*
* Overview:     Resets previous error (integral sum) for a new setpoint.
*		
********************************************************************/
void PI_Clear(void)
{
    previous_error=0;
}