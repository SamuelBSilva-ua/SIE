#include "../includes/PIController.h"
#include "../includes/pwm.h"
#include "../includes/uart.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

volatile uint8_t setpoint;
volatile uint8_t rpm;
volatile float Kp_value=0.0025;
volatile float Ki_value=0.05;
volatile float previous_error=0;



/* PI Controller */
void PI_Execute(uint8_t setpoint, uint8_t rpm)
{

    //printf("\n\n\n\r RPM: %d", rpm);
    //printf("\n\r Setpoint: %d", setpoint);
	float error = 0;
	float pterm = 0, iterm = 0;
    int adjustment;
    
    /* Error calculation */
    error = setpoint - rpm;
    
    /* Error Sum for integrator */
    previous_error += error;
    //printf("\n\r Sum error: %6.2f", previous_error);
    
    /* Proporcional term */
    pterm=Kp_value * error; 
    //printf("\n\r pterm: %6.2f", pterm);
    
    /* Integral term*/
    iterm= previous_error*Ki_value;
    //printf("\n\r iterm: %6.2f", iterm);
    
    
	// calculate the control variable
	adjustment= (int)(pterm + iterm);
    
    //adjustment
    //printf("\n\rAdj Value: %d", adjustment);
    //printf("\n\rPR2: %d", PR2+adjustment);    
    
    
    //saturation  
    if((PR2+adjustment) > PR2){ //if adjustment needed causes value greater than PR2 value, saturation to PR2
        PWM1_updateDutyCycle(100,2); //set duty-cycle to 100%
        //PutString("\n\r 100% dutycycle");
        adjustment=0;
    }else if((PR2-adjustment) < 0){ //if adjustment needed is causes value less than 0, saturation to 0
        PWM1_updateDutyCycle(0,2); //set duty-cycle to 0%
        //PutString("\n\r 0% dutycycle");
        adjustment=0;
    }else{
        PWM1_adjustment(adjustment,2); //Adjust OC1RS counter
        //PutString("\n\r Adjust");
    }
    
    
    //printf("\n\rOC1: %d", OC1RS);         
}

/* Update KP value */
void UpdateKp(float Kp){
    Kp_value=Kp;
}


/* Update KI value*/
void UpdateKi(float Ki){
    Ki_value=Ki;
}


/* Reset previous error for new setpoint*/
void PI_Clear(void)
{
    previous_error=0;
}