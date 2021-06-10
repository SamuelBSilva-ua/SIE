#include "../includes/PIController.h"
#include "../includes/pwm.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

volatile uint8_t setpoint;
volatile uint8_t rpm;
volatile float Kp_value;
volatile float Ki_value;
volatile float previous_error=0;

void PI_Execute(uint8_t setpoint, uint8_t rpm)
{
    Kp_value=0.25;
    Ki_value=1.2;
    
	// Error calculation
	float error = 0;
	float pterm = 0, iterm = 0;
    int adjustment;
    
    uint8_t pwm_value=0;
    
    error = setpoint - rpm;
    
   // erro minimo  -57
    //erro máximo  50
    
    previous_error += error;
    
    pterm=Kp_value * error;
    iterm= previous_error*Ki_value;
	//pi integral minimo = 
    
    
	// calculate the control variable
	adjustment= (int)(pterm + iterm);
      
    printf("\n\rAdj Value: %d", adjustment);

    PWM1_adjustment(adjustment,2);
     
}


void UpdateKp(float Kp){
    Kp_value=Kp;
}



void UpdateKi(float Ki){
    Ki_value=Ki;
}