#include "../inc/config_bits.h" 
#include "../inc/pwm.h"
#include "../inc/timers.h"
#include "../inc/uart.h"
#include "../inc/GP2Y0A21YK0F.h"
#include "../inc/general.h"
#include "../inc/ADC.h"
#include <xc.h>
#include <sys/attribs.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>




/* Interruption Vectors */
#define UART1_VECTOR 24
#define TIMER3_VECTOR 12

/* System variables*/
volatile uint8_t pwm_value;


void __ISR(TIMER3_VECTOR) Timer3ISR(void);



int main(int argc, char** argv) {

    /* Set Interrupt Controller for multi-vector mode */
    INTCONSET = _INTCON_MVEC_MASK;
    
    /* Enable Interrupt Exceptions */
    // set the CP0 status IE bit high to turn on interrupts globally
    __builtin_enable_interrupts();
    
    
    //Init values
    PinConfig();
    Timer2Init(2000,8); //PWM at 20KHz
    PWM2_updateDutyCycle(100,2); //Set PWM to start with 0% duty-cycle
    Timer3Init(10,256); //Set timer3 to 10Hz
    ADC1Init(); //Init ADC
      
    
    /* UART and INT init*/
    UartInit(PBCLK, 115200);   
    __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
    
    PutString("\r---- Projeto B: Distancia ---------\n\n");
    
    while(1);
    return (EXIT_SUCCESS);
}



void __ISR(TIMER3_VECTOR)Timer3ISR(void){

    //Reads the ADC voltage value
  float ADC_value=ADC1Read();
  //Converts voltage to distance with cubic interpolaiton
  float Measure_Distance=CubicInterpolation(ADC_value);
 
  
  if(Measure_Distance == -1){ //no object found
        PutString("\n\r Distancia: E");
        PORTEbits.RE8 = 0; //turn off LED
        PWM1_updateDutyCycle(0,2); //set duty-cycle to 0
  }else{
        printf("\n\rDistancia: %6.0f mm", Measure_Distance);
        PORTEbits.RE8 = 1; //turn on LED
        pwm_value=(uint8_t)(0.2*Measure_Distance-20); //PWM value in percentage
        printf("        PWM_value: %d", pwm_value);
        PWM1_updateDutyCycle(pwm_value,2); //set new PWM value
  }

  IFS0bits.T3IF = 0; //reset interrupt flag
}